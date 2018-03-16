#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h> 
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "pixel.hpp"
#include "bitmap.hpp"

void error_out(char const * const message, int const code) {
    fputs(message, stderr);
    exit(code);
}

struct Framebuffer {
    int file;
    struct fb_fix_screeninfo info_fix;
    struct fb_var_screeninfo info_var;
    size_t size;
    uint8_t * buffer;

    void update_info_var() {
        if (ioctl(file, FBIOGET_VSCREENINFO, &info_var))
            error_out("Error reading variable info\n", 3);
    }

    Framebuffer(char const * const filename) {
        file = open(filename, O_RDWR);
        if (file == 0)
            error_out("Error opening device file\n", 1);

        if (ioctl(file, FBIOGET_FSCREENINFO, &info_fix))
            error_out("Error reading fixed info\n", 2);
        
        update_info_var();

        size = (
            info_var.xres *
            info_var.yres *
            info_var.bits_per_pixel
        ) / 8;
        
        buffer = reinterpret_cast<uint8_t *>(
            mmap(
                0, size,
                PROT_READ | PROT_WRITE, MAP_SHARED,
                file, 0
            )
        );
        if (reinterpret_cast<long>(buffer) == -1)
            error_out("Error mapping device to memory", 4);
    }

    ~Framebuffer() {
        munmap(buffer, size);
        close(file);
    }

    void print_info() {
        printf(
            "Framebuffer: [ %dx%d ], %d bits per pixel\n",
            info_var.xres, info_var.yres,
            info_var.bits_per_pixel
        );
    }

    uint8_t * buffer_location(size_t const x, size_t const y) {
        return buffer +
            (x + info_var.xoffset) * (info_var.bits_per_pixel / 8) +
            (y + info_var.yoffset) * (info_fix.line_length);
    }

    void write_pixel(size_t const x, size_t const y, Pixel const color) {
        auto const location = buffer_location(x, y);
        auto const pixel32 = reinterpret_cast<Pixel *>(location);
        *pixel32 = color;
    }
    
    void write_line(size_t const y, Pixel const * const pixels, int startx, int width) {
        size_t num_bytes{width * sizeof(Pixel)};
        uint8_t * line_start{buffer_location(0, y)};
        memcpy(line_start, pixels, num_bytes);
    }

    template<typename T, typename U>
    T real_modulo(T lhs, U rhs) {
        return ((lhs % rhs) + rhs) % rhs;
    }

    void write_fullscreen(Bitmap const & bitmap, size_t startx, size_t starty) {
        size_t const
            screen_width{info_var.xres},
            screen_height{info_var.yres};
        
        for (int screen_y{0}; screen_y < screen_height; screen_y++) { 
            for (int screen_x{0}; screen_x < screen_width; screen_x++) {
                size_t
                    bitmap_x = startx + screen_x,
                    bitmap_y = starty + screen_y;
                
                Pixel screen_color = bitmap.read_pixel(bitmap_x, bitmap_y);
                write_pixel(screen_x, screen_y, screen_color);
            }
         }
    }
};


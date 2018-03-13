#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h> 
#include <sys/mman.h>
#include <sys/ioctl.h>

void error_out(char const * const message, int const code) {
    fputs(message, stderr);
    exit(code);
}

struct Bgra {  //Blue, Green, Red, Alpha (opacity) --default format for BITMAP images
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
};

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
            (y + info_var.yoffset) * (info_fix.line_length       );
    }

    void write_pixel(size_t const x, size_t const y, Bgra const color) {
        auto const location = buffer_location(x, y);
        auto const pixel32 = reinterpret_cast<Bgra *>(location);
        auto const pixel16 = reinterpret_cast<uint16_t *>(location);

        switch (info_var.bits_per_pixel) {
        case 32:
            *pixel32 = color;
            break;

        case 16:
        default:
            *pixel16 = (
                (color.r / 16) << 11 |
                (color.g / 16) << 5  |
                (color.b / 16) << 0
            );
            break;
        }
    }
    
    void write_line(size_t const y, Bgra const * const pixels, size_t const width) {
        size_t num_bytes{width * sizeof(Bgra)};
        uint8_t * line_start{buffer_location(0, y)};
        memcpy(line_start, pixels, num_bytes);
    }
};


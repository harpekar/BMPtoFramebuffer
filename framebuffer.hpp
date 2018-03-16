#include <stdexcept>
#include <cstdint>
#include <cerrno>
#include <cstring>
#include <iostream>

#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h> 
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "pixel.hpp"
#include "bitmap.hpp"

struct Framebuffer {
private:
    int file;
    struct fb_fix_screeninfo info_fix;
    struct fb_var_screeninfo info_var;
    size_t size;
    uint8_t * buffer;

    uint8_t * buffer_location(size_t const x, size_t const y) {
        return buffer +
            (x + info_var.xoffset) * (info_var.bits_per_pixel / 8) +
            (y + info_var.yoffset) * (info_fix.line_length);
    }

public:
    void update_info_var() {
        if (ioctl(file, FBIOGET_VSCREENINFO, &info_var))
            throw std::runtime_error(std::strerror(errno));
    }

    Framebuffer(char const * const filename) {
        file = open(filename, O_RDWR);
        if (file == 0)
            throw std::runtime_error(std::strerror(errno));

        if (ioctl(file, FBIOGET_FSCREENINFO, &info_fix)) {
            throw std::runtime_error(std::strerror(errno));
        }
        
        update_info_var();

        size = (
            info_var.xres *
            info_var.yres *
            (info_var.bits_per_pixel / 8)
        );
        
        buffer = reinterpret_cast<uint8_t *>(
            mmap(
                0, size,
                PROT_READ | PROT_WRITE, MAP_SHARED,
                file, 0
            )
        );
        if (reinterpret_cast<long>(buffer) == -1)
            throw std::runtime_error("Error mapping device to memory");
    }

    ~Framebuffer() {
        munmap(buffer, size);
        close(file);
    }

    uint32_t get_x_res()     const { return info_var.xres;           }
    uint32_t get_y_res()     const { return info_var.yres;           }
    uint32_t get_bit_depth() const { return info_var.bits_per_pixel; }

    void print_info() const {
        printf(
            "Framebuffer: [ %dx%d ], %d bits per pixel\n",
            info_var.xres, info_var.yres,
            info_var.bits_per_pixel
        );
    }

    void write_pixel(size_t const x, size_t const y, Pixel const color) {
        auto const location = buffer_location(x, y);
        auto const pixel32 = reinterpret_cast<Pixel *>(location);
        *pixel32 = color;
    }

    void write_fullscreen(Bitmap const & bitmap, ssize_t startx, ssize_t starty) {
        size_t const
            screen_width{info_var.xres},
            screen_height{info_var.yres};
        
        for (int screen_y{0}; screen_y < screen_height; screen_y++) { 
            for (int screen_x{0}; screen_x < screen_width; screen_x++) {
                ssize_t
                    bitmap_x{startx + screen_x},
                    bitmap_y{starty + screen_y};
                
                Pixel screen_color = bitmap.read_pixel(bitmap_x, bitmap_y);
                write_pixel(screen_x, screen_y, screen_color);
            }
         }
    }
};


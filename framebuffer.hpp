#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <stdexcept>
#include <cstdint>
#include <cerrno>
#include <cstring>

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
            (info_var.xoffset + x) * (info_var.bits_per_pixel / 8) +
            (info_var.yoffset + y) * (info_fix.line_length);
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
        auto const pixel16 = reinterpret_cast<uint16_t*>(location);
    
    switch(info_var.bits_per_pixel) {
    case 32: 
        *pixel32 = color;
        break;

    case 16:
    default:
        *pixel16 = (
            ((color.r >> 3) << 11) | // 5 bits red
            ((color.g >> 2) <<  5) | // 6 bits green
            ((color.b >> 3) <<  0)   // 5 bits blue
        );
        break;   
    }

    }

    void write_fullscreen(Bitmap const & bitmap, ssize_t start_x, ssize_t start_y) {
     //   printf(
     //       "Coordinates %d mod %d = %d\n",
     //       start_x,
     //       bitmap.get_x_res(),
     //       start_x % bitmap.get_x_res()
     //   );
        for (ssize_t screen_y{0}; screen_y < 320; screen_y++) { 
            for (ssize_t screen_x{0}; screen_x < info_var.xres; screen_x++) {
                write_pixel(
                    screen_x, screen_y,
                    bitmap.read_pixel(
                        start_x + screen_x,
                        start_y + screen_y
                    )
                );
            }
         }
    }
};

#endif

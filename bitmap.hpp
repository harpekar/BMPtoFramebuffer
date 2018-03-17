#ifndef BITMAP_H
#define BITMAP_H

#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdint>

#include "pixel.hpp"

constexpr static Pixel const MAGENTA { 255, 0, 255, 255 };

struct Bitmap {
private:
    enum bitmap_index {
        SIZE    = 0x02,
        OFFSET  = 0x0A,
        WIDTH   = 0x12,
        HEIGHT  = 0x16,
        PIXSIZE = 0x1C,
    };

    uint32_t x_resolution;
    uint32_t y_resolution;
    std::vector<Pixel> pixels;

    template<typename ReadType>
    static ReadType read_at(std::ifstream & file, std::streampos const offset) {
        ReadType buffer;
        file.seekg(offset);
        file.get(reinterpret_cast<char *>(&buffer), sizeof(ReadType));
        return buffer;
    }

    // template<typename T, typename U>
    // static T real_modulo(T lhs, U rhs) {
    //     return ((lhs % rhs) + rhs) % rhs;
    // }

public:
    Bitmap(char const * const filename) {
        std::ifstream file{filename, std::ios::binary};
        
        x_resolution = read_at<uint32_t>(file, bitmap_index::WIDTH);
        y_resolution = read_at<uint32_t>(file, bitmap_index::HEIGHT);

        if (x_resolution == 0 || y_resolution == 0)
            throw std::runtime_error("Failed to load image: dimensions are zero");

        uint32_t const image_offset{
            read_at<uint32_t>(file, bitmap_index::OFFSET)
        };
        file.seekg(image_offset);
        
        size_t const num_pixels = x_resolution * y_resolution;
        // Bitmaps are word-aligned
        size_t const line_bytes = x_resolution * 3;
        size_t const padding_bytes{(4 - (line_bytes % 4)) % 4};

        for (size_t y{0}; y < y_resolution; y++) {
            for (size_t x{0}; x < x_resolution; x++) {
                Pixel pixel;

                pixel.b = file.get();
                pixel.g = file.get();
                pixel.r = file.get();
                pixel.a = 255; //file.get();

                pixels.push_back(pixel);
            }
            for (size_t i{0}; i < padding_bytes; i++) {
                // Discard padding bytes
                file.get();
            }
        }
    }

    uint32_t get_x_res() const { return x_resolution; }
    uint32_t get_y_res() const { return y_resolution; }
    
    Pixel read_pixel(ssize_t x, ssize_t y) const {
        // Vertical clipping
        if (y >= y_resolution || y < 0)
            return MAGENTA;

        // Wrapping
        x %= x_resolution;
        // Vertical inversion
        y = y_resolution - y;
        
        return pixels[(y * x_resolution) + x];
    }       
};   

#endif

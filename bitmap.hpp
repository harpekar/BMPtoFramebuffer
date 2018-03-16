#ifndef BITMAP_H
#define BITMAP_H

#include <fstream>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "pixel.hpp"

constexpr static Pixel const MAGENTA { 255, 0, 255, 255 };

struct Bitmap {
    enum bitmap_index {
        SIZE    = 0x02,
        OFFSET  = 0x0A,
        WIDTH   = 0x12,
        HEIGHT  = 0x16,
        PIXSIZE = 0x1C,
    };

    uint32_t x;
    uint32_t y;
    std::vector<Pixel> pixels;

    template<typename ReadType>
    static ReadType read_at(std::ifstream & file, std::streampos const offset) {
        ReadType buffer;
        file.seekg(offset);
        file.get(reinterpret_cast<char *>(&buffer), sizeof(ReadType));
        return buffer;
    }

    Bitmap(char const * const filename) {
        std::ifstream file{filename, std::ios::binary};
        
        x = read_at<uint32_t>(file, bitmap_index::WIDTH);
        y = read_at<uint32_t>(file, bitmap_index::HEIGHT);

        if (x == 0 || y == 0) {
            printf("Failed to load image: dimensions are zero\n");
            exit(1);
        }

        uint32_t image_offset = read_at<uint32_t>(file, bitmap_index::OFFSET);
        file.seekg(image_offset + 1);
        
        size_t const num_pixels = x * y;
        // Bitmaps are word-aligned
        size_t const line_bytes = x * 3;
        size_t const padding_bytes{(4 - (line_bytes % 4)) % 4};

        for (size_t y_iter{0}; y_iter < y; y_iter++) {
            for (size_t x_iter{0}; x_iter < x; x_iter++) {
                Pixel pixel;

                pixel.g = file.get();
                pixel.r = file.get();
                pixel.b = file.get();
                pixel.a = 255; //file.get();

                pixels.push_back(pixel);
            }
            for (size_t i{0}; i < padding_bytes; i++) {
                file.get();
            }
        }

        puts("Pixels:");
        for (int i{0}; i < 4; i++) {
            Pixel p = pixels[i];
            printf("%X %X %X\n", p.r, p.b, p.g);
        }
    }

    Pixel read_pixel(size_t input_x, size_t input_y) const {
        if (input_y > y) return MAGENTA; // Clipping

        input_x %= x; // Wrapping
        input_y = y - input_y; // Inversion
        
        return pixels[(input_y * x) + input_x + 1];
    }       
};   

#endif
#include <fstream>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "framebuffer.hpp"

using namespace std; 

namespace bitmap_index {
    size_t const SIZE  {0x02};
    size_t const OFFSET{0x0A};
    size_t const WIDTH {0x12};
    size_t const HEIGHT{0x16};
    size_t const PIXSIZE{0x1C};
}

template<typename ReadType>
ReadType read_at(ifstream & file, streampos const offset) {
    ReadType buffer;
    file.seekg(offset);
    file.get(reinterpret_cast<char *>(&buffer), sizeof(ReadType));
    return buffer;
}

struct Bitmap {
    uint32_t x;
    uint32_t y;
    vector<Bgra> pixels;

    Bitmap(char const * const filename) {
        ifstream file{filename, ios::binary};
        
        x = read_at<uint32_t>(file, bitmap_index::WIDTH);
        y = read_at<uint32_t>(file, bitmap_index::HEIGHT);

        uint32_t image_offset = read_at<uint32_t>(file, bitmap_index::OFFSET);
        file.seekg(image_offset);
        
        for (size_t _{0}; _ < x * y; _++) {
            Bgra pixel;

            pixel.b = file.get();
            pixel.g = file.get();
            pixel.r = file.get();
            pixel.a = 255;

            pixels.push_back(pixel);
        }
    }       
};

int main(int argc, char* argv[]){
    if (argc < 1) {
        printf("Please pass in a file-image then framebuffer\n");
        exit(1);
    }

    Bitmap bitmap{argv[1]};

    Framebuffer fb{argv[2]};
    
    for (size_t i{0}; i< bitmap.y; i++)
        fb.write_line(
            bitmap.y - i,
            &bitmap.pixels[i*bitmap.x],
            bitmap.x
        );

    return 0;
}

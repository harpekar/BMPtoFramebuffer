#include <iostream>
#include <cstddef>

#include "bitmap.hpp"
#include "framebuffer.hpp"

int main(int argc, char* argv[]){
    ssize_t
        startx{-200},
        starty{0};
    
    if (argc < 3) {
        std::cout << "Please pass in a file-image then framebuffer\n";
        exit(1);
    }

    Framebuffer fb{argv[2]}; 
    Bitmap bitmap{argv[1]};
    
    fb.write_fullscreen(bitmap, startx, starty);

    return 0;
}

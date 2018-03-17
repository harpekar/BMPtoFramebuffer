#include <iostream>
#include <cstddef>

#include "bitmap.hpp"
#include "framebuffer.hpp"
#include "imageviewer.hpp"

int main(int argc, char* argv[]){
    ssize_t
        startx{0},
        starty{0};
    
    if (argc < 3) {
        std::cout << "Please pass in a file-image then framebuffer\n";
        exit(1);
    }

    ImageViewer im{argv[2],argv[1]};

    im.draw();

    return 0;
}

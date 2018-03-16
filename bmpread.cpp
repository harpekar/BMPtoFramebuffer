#include <fstream>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "bitmap.hpp"
#include "framebuffer.hpp"

using namespace std; 

int main(int argc, char* argv[]){
    int starty = 0;
    int startx = 0;
    
    if (argc < 1) {
        printf("Please pass in a file-image then framebuffer\n");
        exit(1);
    }
    puts("Opening bitmap...");
    Bitmap bitmap{argv[1]};

    puts("Opening framebuffer...");
    Framebuffer fb{argv[2]}; 
    
    puts("Writing image...");
    fb.write_fullscreen(bitmap, startx, starty);

    return 0;
}

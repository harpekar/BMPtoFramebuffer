#include <iostream>
#include <cstddef>
#include <wiringPi.h>

#include "bitmap.hpp"
#include "framebuffer.hpp"
#include "imageviewer.hpp"
#include "encoder.hpp"

#define BUTTON 26 

int main(int argc, char* argv[]){
    wiringPiSetup();
    pinMode(BUTTON, INPUT);
    pullUpDnControl(BUTTON, PUD_UP);

    ssize_t
        startx{0},
        starty{0};
   
    bool axis = true; 
    if (argc < 3) {
        std::cout << "Please pass in a file-image then framebuffer\n";
        exit(1);
    }
    
    ImageViewer image_viewer{ argv[2], argv[1] };
    Encoder encoder;

    image_viewer.draw();
    
    while(true) {
        if (digitalRead(BUTTON) == 0)
        {
            axis = !axis; 
        } 
        image_viewer.scroll(encoder.detect_turn(axis));
        usleep(50);
    }
}

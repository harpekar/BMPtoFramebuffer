#ifndef ENCODER_HPP
#define ENCODER_HPP

#include <wiringPi.h>
#include <cstdio>

#include "imageviewer.hpp"

struct Encoder { 
private:
    static int const 
        DATA = 29,
        CLOCK = 28,
        POWER = 27;

    int clkLastState;

public:
    Encoder() {
        wiringPiSetup();

        pinMode(POWER, OUTPUT);
        pullUpDnControl(POWER, PUD_UP);
        
        pinMode(DATA, INPUT);
        pullUpDnControl(DATA, PUD_DOWN);
        pinMode(CLOCK, INPUT);
        pullUpDnControl(CLOCK, PUD_DOWN);

        clkLastState = 0;
    }
    
    ImageViewer::Direction detect_turn(bool axis) {
        int clockState = digitalRead(CLOCK);
        int dtState = digitalRead(DATA);

        if (clockState == clkLastState)
            return ImageViewer::Direction::NONE;

        clkLastState = clockState;

        if (axis == true) { //Horizontal axis
        if (dtState == clockState) { 
            return ImageViewer::Direction::LEFT; 
        } else {
            return ImageViewer::Direction::RIGHT; 
        }
        }

        else if (axis == false) { //Vertical axis
      if (dtState == clockState) { 
                return ImageViewer::Direction::UP; 
            } else {
                return ImageViewer::Direction::DOWN; 
           
            }
        }
        else 
            return ImageViewer::Direction::NONE;
    }
};

#endif

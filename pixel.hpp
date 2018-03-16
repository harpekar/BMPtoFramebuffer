#ifndef PIXEL_H
#define PIXEL_H

#include <stdint.h>

struct Pixel {  //Blue, Green, Red, Alpha (opacity) --default format for BITMAP images
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
};

#endif

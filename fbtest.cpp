#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "framebuffer.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2)
        error_out("Please provide a framebuffer file (/dev/fb0)\n", 1);

    Framebuffer fb{argv[1]};
    fb.print_info();

    struct { size_t x; size_t y; } pos;
    for (pos.x = 100; pos.x < 200; pos.x++) {
        for (pos.y = 100; pos.y < 200; pos.y++) {
            Bgra color { 100, 150, 255, 100 };
            fb.write_pixel(pos.x, pos.y, color);
        }
    }

    return 0;
}

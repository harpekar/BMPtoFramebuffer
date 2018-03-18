#ifndef IMAGE_VIEWER_H
#define IMAGE_VIEWER_H

#include "framebuffer.hpp"
#include "bitmap.hpp"

struct ImageViewer {
private:
    Framebuffer framebuffer;
    Bitmap bitmap;
    struct { ssize_t x; ssize_t y; } position{ 0, 0 };

    static size_t const STEP{20}; // Step size of 20px

public:
    enum struct Direction { NONE, DOWN, UP, LEFT, RIGHT };

    ImageViewer(
        char const * const fb_filename,
        char const * const bmp_filename
    ) :
        framebuffer{Framebuffer{fb_filename}},
        bitmap{Bitmap{bmp_filename}}
    {}

    void draw() {
        framebuffer.write_fullscreen(bitmap, position.x, position.y);
    }

    void scroll(Direction direction) {
        switch (direction) {
        case Direction::NONE:
            return;

        case Direction::DOWN:
            if (framebuffer.get_y_res() > bitmap.get_y_res())
                position.y = std::max<size_t>(position.y - STEP, 0);
            break;

        case Direction::UP:
            if (framebuffer.get_y_res() > bitmap.get_y_res())
                position.y = std::min<size_t>(
                    position.y + STEP,
                    bitmap.get_y_res() - framebuffer.get_y_res()
                );
            break;

        case Direction::LEFT:
            position.x -= STEP;
            break;

        case Direction::RIGHT:
            position.x += STEP;
            break;
        }
        draw();
    }
};

#endif

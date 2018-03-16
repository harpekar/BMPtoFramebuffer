#ifndef IMAGE_VIEWER_H
#define IMAGE_VIEWER_H

#include "framebuffer.hpp"
#include "bitmap.hpp"

struct ImageViewer {
private:
    Framebuffer framebuffer;
    Bitmap bitmap;
    struct { ssize_t x; ssize_t y; } scroll{ 0, 0 };

    static size_t const STEP{20}; // Step size of 20px

public:
    enum struct Direction { DOWN, UP, LEFT, RIGHT };

    ImageViewer(
        char const * const fb_filename,
        char const * const bmp_filename
    ) :
        framebuffer{Framebuffer{fb_filename}},
        bitmap{Bitmap{bmp_filename}}
    {}

    void update() {
        framebuffer.write_fullscreen(bitmap, scroll.x, scroll.y);
    }

    void scroll(Direction direction) {
        switch (direction) {
        case Direction::DOWN:
            if (framebuffer.get_y_res() > bitmap.get_y_res())
                scroll.y = std::max<size_t>(scroll.y - STEP, 0);
            break;

        case Direction::UP:
            if (framebuffer.get_y_res() > bitmap.get_y_res())
                scroll.y = std::min<size_t>(
                    scroll.y + STEP,
                    bitmap.get_y_res() - framebuffer.get_y_res()
                );
            break;

        case Direction::LEFT:
            scroll.x -= STEP;
            break;

        case Direction::RIGHT:
            scroll.x += STEP;
            break;
        }
        update();
    }
};

#endif
#ifndef RECT_HPP_
#define RECT_HPP_
#include <switch.h>
class Rect {
    public:
        u32 x;
        u32 y;
        u32 x1;
        u32 y1;
        u32 rgba;

        Rect(u32 x, u32 y, u32 x1, u32 y1, u32 rgba) {
            this->x = x;
            this->y = y;
            this->x1 = x1;
            this->y1 = y1;
            this->rgba = rgba;
        }
};
#endif
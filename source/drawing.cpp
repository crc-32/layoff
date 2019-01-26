#include <switch.h>
#include "drawing.hpp"

using namespace std;
void Drawing::Setup() {
    if (R_FAILED(viOpenDefaultDisplay(&this->display))) {
        fatalSimple(MAKERESULT(360, 1));
    }
    u32 display_width, display_height;
    if (R_FAILED(viGetDisplayLogicalResolution(&this->display, &display_width, &display_height))) {
        fatalSimple(MAKERESULT(360, 2));
    }
    if (R_FAILED(viSetDisplayMagnification(&this->display, 0, 0, display_width, display_height))) {
        fatalSimple(MAKERESULT(360, 3));
    }

    if (R_FAILED(viCreateLayer(&this->display, &this->layer))) {
        fatalSimple(MAKERESULT(360, 4));
    }
    u64 maxZ;
    viGetDisplayMaximumZ(&this->display, &maxZ);
    viSetLayerZ(&this->layer, maxZ);

    if (R_FAILED(nwindowCreateFromLayer(&this->win, &this->layer))) {
        fatalSimple(MAKERESULT(360, 5));
    }
    if (R_FAILED(framebufferCreate(&this->fb, &this->win, 1280, 720, PIXEL_FORMAT_RGBA_8888, 2))) {
        fatalSimple(MAKERESULT(360, 6));
    }
    framebufferMakeLinear(&this->fb);
}

void Drawing::Exit() {
    framebufferClose(&this->fb);
    viCloseLayer(&this->layer);
    viCloseDisplay(&this->display);
}

void Drawing::Test() {
    u32 stride;
    u32 *linBuf = reinterpret_cast<u32 *>(framebufferBegin(&this->fb, &stride));
    if (linBuf == nullptr) {
        fatalSimple(MAKERESULT(360, 0));
    }
    for (u32 y = 0; y < 720/2; y ++)
        {
            for (u32 x = 0; x < 1280; x ++)
            {
                u32 pos = y * stride / sizeof(u32) + x;
                linBuf[pos] = 0x010101FF;
            }
    }
    framebufferEnd(&this->fb);
}
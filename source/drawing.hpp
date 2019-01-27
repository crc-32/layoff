#include <switch.h>
#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "notification.hpp"
using namespace std;
class Drawing {
    private:
        ViDisplay display;
        ViLayer layer;
        NWindow win;
        Framebuffer fb;
        vector<Notification*> notifications;
        u32 stride;
        FT_Library library;
        FT_Face face;
        void RectDraw(Rect *rect, u32* linBuf);
        void RenderNotifs(u32* linBuf);
    public:
        Drawing() {};
        void Setup();
        void Exit();
        void Test();
        void Render();
};
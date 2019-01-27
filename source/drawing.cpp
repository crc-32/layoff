#include <switch.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "drawing.hpp"
#include "rect.hpp"

using namespace std;

void draw_glyph(FT_Bitmap* bitmap, u32* framebuf, u32 x, u32 y, u32 stride)
{
    u32 framex, framey;
    u32 tmpx, tmpy;
    u8* imageptr = bitmap->buffer;
    u32 framebuf_width = stride / sizeof(u32);

    if (bitmap->pixel_mode!=FT_PIXEL_MODE_GRAY) return;

    for (tmpy=0; tmpy<bitmap->rows; tmpy++)
    {
        for (tmpx=0; tmpx<bitmap->width; tmpx++)
        {
            framex = x + tmpx;
            framey = y + tmpy;

            framebuf[framey * framebuf_width + framex] = RGBA8_MAXALPHA(imageptr[tmpx], imageptr[tmpx], imageptr[tmpx]);
        }

        imageptr+= bitmap->pitch;
    }
}

//Note that this doesn't handle {tmpx > width}, etc.
//str is UTF-8.
void draw_text(FT_Face face, u32* framebuf, u32 x, u32 y, u32 stride, const char* str)
{
    u32 tmpx = x;
    FT_Error ret=0;
    FT_UInt glyph_index;
    FT_GlyphSlot slot = face->glyph;

    u32 i;
    u32 str_size = strlen(str);
    uint32_t tmpchar;
    ssize_t unitcount=0;

    for (i = 0; i < str_size; )
    {
        unitcount = decode_utf8 (&tmpchar, (const uint8_t*)&str[i]);
        if (unitcount <= 0) break;
        i+= unitcount;

        if (tmpchar == '\n')
        {
            tmpx = x;
            y+= face->size->metrics.height / 64;
            continue;
        }

        glyph_index = FT_Get_Char_Index(face, tmpchar);
        //If using multiple fonts, you could check for glyph_index==0 here and attempt using the FT_Face for the other fonts with FT_Get_Char_Index.

        ret = FT_Load_Glyph(
                face,          /* handle to face object */
                glyph_index,   /* glyph index           */
                FT_LOAD_DEFAULT);

        if (ret==0)
        {
            ret = FT_Render_Glyph( face->glyph,   /* glyph slot  */
                                   FT_RENDER_MODE_NORMAL);  /* render mode */
        }

        if (ret) return;

        draw_glyph(&slot->bitmap, framebuf, tmpx + slot->bitmap_left, y - slot->bitmap_top, stride);

        tmpx += slot->advance.x >> 6;
        y += slot->advance.y >> 6;
    }
}

void Drawing::Setup() {
    u64 maxZ;

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

    viSetDisplayAlpha(&this->display, 1.0f);

    if (R_FAILED(viCreateLayer(&this->display, &this->layer))) {
        fatalSimple(MAKERESULT(360, 4));
    }
    viGetDisplayMaximumZ(&this->display, &maxZ);
    viSetLayerZ(&this->layer, maxZ);

    if (R_FAILED(nwindowCreateFromLayer(&this->win, &this->layer))) {
        fatalSimple(MAKERESULT(360, 5));
    }

    if (R_FAILED(framebufferCreate(&this->fb, &this->win, 1280, 720, PIXEL_FORMAT_RGBA_8888, 1))) {
        fatalSimple(MAKERESULT(360, 7));
    }
    framebufferMakeLinear(&this->fb);


    Result rc=0;
    FT_Error ret=0;
    PlFontData font;
    rc = plGetSharedFontByType(&font, PlSharedFontType_Standard);
    if (R_FAILED(rc))
        fatalSimple(MAKERESULT(360, 8));
    ret = FT_Init_FreeType(&library);
    if (ret)
        fatalSimple(MAKERESULT(360, 9));
    ret = FT_New_Memory_Face( library,
                              (FT_Byte*)font.address,    /* first byte in memory */
                              font.size,       /* size in bytes        */
                              0,               /* face_index           */
                              &face);
    if (ret) {
        FT_Done_FreeType(library);
        fatalSimple(MAKERESULT(360, 10));
    }


}

void Drawing::Exit() {
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    framebufferClose(&this->fb);
    viCloseLayer(&this->layer);
    viCloseDisplay(&this->display);
}

void Drawing::Test() {
    this->notifications.push_back(new Notification("Notification"));
}

void Drawing::RectDraw(Rect *rect, u32* linBuf) {
    if (linBuf == nullptr) {
        fatalSimple(MAKERESULT(360, 2));
    }
    for (u32 y = rect->y; y < rect->y1; y++) {
        for (u32 x = rect->x; x < rect->x1; x++) {
            u32 pos = y * this->stride / sizeof(u32) + x;
            linBuf[pos] = rect->rgba;
        }
    }
}
void Drawing::RenderNotifs(u32* linBuf) {
    for (int i = 0; i < (int) this->notifications.size(); i++) {
        RectDraw(notifications[i]->rect, linBuf);
        FT_Error ret;
        ret = FT_Set_Char_Size(
            face,    /* handle to face object           */
            0,       /* char_width in 1/64th of points  */
            18*64,   /* char_height in 1/64th of points */
            96,      /* horizontal device resolution    */
            96);     /* vertical device resolution      */
        if (ret) {
            FT_Done_Face(face);
            FT_Done_FreeType(library);
            fatalSimple(MAKERESULT(360, 11));
        }
        draw_text(this->face, linBuf, 1, (notifications[i]->rect->y1/2 + (18/2)), this->stride, notifications[i]->title.c_str());
    }
}

void Drawing::Render() {
    u32 stride;
    u32* linBuf = (u32*) framebufferBegin(&this->fb, &stride);
    this->stride = stride;
    this->RenderNotifs(linBuf);
    framebufferEnd(&this->fb);
}
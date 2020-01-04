#include "imgui_freetype.h"
#include "imgui_internal.h"     // ImMin,ImMax,ImFontAtlasBuild*,
#include <stdint.h>
#include <ft2build.h>
#include FT_FREETYPE_H          // <freetype/freetype.h>
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-function"          // warning: 'xxxx' defined but not used
#endif
namespace
{
    // Glyph metrics:
    // --------------
    //
    //                       xmin                     xmax
    //                        |                         |
    //                        |<-------- width -------->|
    //                        |                         |
    //              |         +-------------------------+----------------- ymax
    //              |         |    ggggggggg   ggggg    |     ^        ^
    //              |         |   g:::::::::ggg::::g    |     |        |
    //              |         |  g:::::::::::::::::g    |     |        |
    //              |         | g::::::ggggg::::::gg    |     |        |
    //              |         | g:::::g     g:::::g     |     |        |
    //    offsetX  -|-------->| g:::::g     g:::::g     |  offsetY     |
    //              |         | g:::::g     g:::::g     |     |        |
    //              |         | g::::::g    g:::::g     |     |        |
    //              |         | g:::::::ggggg:::::g     |     |        |
    //              |         |  g::::::::::::::::g     |     |      height
    //              |         |   gg::::::::::::::g     |     |        |
    //  baseline ---*---------|---- gggggggg::::::g-----*--------      |
    //            / |         |             g:::::g     |              |
    //     origin   |         | gggggg      g:::::g     |              |
    //              |         | g:::::gg   gg:::::g     |              |
    //              |         |  g::::::ggg:::::::g     |              |
    //              |         |   gg:::::::::::::g      |              |
    //              |         |     ggg::::::ggg        |              |
    //              |         |         gggggg          |              v
    //              |         +-------------------------+----------------- ymin
    //              |                                   |
    //              |------------- advanceX ----------->|

    /// A structure that describe a glyph.
    struct GlyphInfo
    {
        int         Width;              // Glyph's width in pixels.
        int         Height;             // Glyph's height in pixels.
        FT_Int      OffsetX;            // The distance from the origin ("pen position") to the left of the glyph.
        FT_Int      OffsetY;            // The distance from the origin to the top of the glyph. This is usually a value < 0.
        float       AdvanceX;           // The distance from the origin to the origin of the next glyph. This is usually a value > 0.
    };

    // Font parameters and metrics.
    struct FontInfo
    {
        uint32_t    PixelHeight;        // Size this font was generated with.
        float       Ascender;           // The pixel extents above the baseline in pixels (typically positive).
        float       Descender;          // The extents below the baseline in pixels (typically negative).
        float       LineSpacing;        // The baseline-to-baseline distance. Note that it usually is larger than the sum of the ascender and descender taken as absolute values. There is also no guarantee that no glyphs extend above or below subsequent baselines when using this distance. Think of it as a value the designer of the font finds appropriate.
        float       LineGap;            // The spacing in pixels between one row's descent and the next row's ascent.
        float       MaxAdvanceWidth;    // This field gives the maximum horizontal cursor advance for all glyphs in the font.
    };

    // FreeType glyph rasterizer.
    // NB: No ctor/dtor, explicitly call Init()/Shutdown()
    struct FreeTypeFont
    {
        bool                    InitFont(FT_Library ft_library, const ImFontConfig& cfg, unsigned int extra_user_flags); // Initialize from an external data buffer. Doesn't copy data, and you must ensure it stays valid up to this object lifetime.
        void                    CloseFont();
        void                    SetPixelHeight(int pixel_height); // Change font pixel size. All following calls to RasterizeGlyph() will use this size
        const FT_Glyph_Metrics* LoadGlyph(uint32_t in_codepoint);
        const FT_Bitmap*        RenderGlyphAndGetInfo(GlyphInfo* out_glyph_info);
        void                    BlitGlyph(const FT_Bitmap* ft_bitmap, uint8_t* dst, uint32_t dst_pitch, unsigned char* multiply_table = NULL);
        ~FreeTypeFont()         { CloseFont(); }

        // [Internals]
        FontInfo        Info;               // Font descriptor of the current font.
        FT_Face         Face;
        unsigned int    UserFlags;          // = ImFontConfig::RasterizerFlags
        FT_Int32        LoadFlags;
    };

    // From SDL_ttf: Handy routines for converting from fixed point
    #define FT_CEIL(X)  (((X + 63) & -64) / 64)
}

#ifndef STB_RECT_PACK_IMPLEMENTATION        // in case the user already have an implementation in the _same_ compilation unit (e.g. unity builds)
#define STBRP_ASSERT(x)    IM_ASSERT(x)
#define STBRP_STATIC
#define STB_RECT_PACK_IMPLEMENTATION
#include "imstb_rectpack.h"
#endif

struct ImFontBuildSrcGlyphFT
{
    GlyphInfo           Info;
    uint32_t            Codepoint;
    unsigned char*      BitmapData;         // Point within one of the dst_tmp_bitmap_buffers[] array
};

struct ImFontBuildSrcDataFT
{
    FreeTypeFont        Font;
    stbrp_rect*         Rects;              // Rectangle to pack. We first fill in their size and the packer will give us their position.
    const ImWchar*      SrcRanges;          // Ranges as requested by user (user is allowed to request too much, e.g. 0x0020..0xFFFF)
    int                 DstIndex;           // Index into atlas->Fonts[] and dst_tmp_array[]
    int                 GlyphsHighest;      // Highest requested codepoint
    int                 GlyphsCount;        // Glyph count (excluding missing glyphs and glyphs already set by an earlier source font)
    ImBoolVector        GlyphsSet;          // Glyph bit map (random access, 1-bit per codepoint. This will be a maximum of 8KB)
    ImVector<ImFontBuildSrcGlyphFT>   GlyphsList;
};

// Temporary data for one destination ImFont* (multiple source fonts can be merged into one destination ImFont)
struct ImFontBuildDstDataFT
{
    int                 SrcCount;           // Number of source fonts targeting this destination font.
    int                 GlyphsHighest;
    int                 GlyphsCount;
    ImBoolVector        GlyphsSet;          // This is used to resolve collision when multiple sources are merged into a same destination font.
};

bool ImFontAtlasBuildWithFreeType(FT_Library ft_library, ImFontAtlas* atlas, unsigned int extra_flags);

// Default memory allocators
static void* ImFreeTypeDefaultAllocFunc(size_t size, void* user_data);
static void  ImFreeTypeDefaultFreeFunc(void* ptr, void* user_data);

// FreeType memory allocation callbacks
static void* FreeType_Alloc(FT_Memory /*memory*/, long size);

static void FreeType_Free(FT_Memory /*memory*/, void* block);

static void* FreeType_Realloc(FT_Memory /*memory*/, long cur_size, long new_size, void* block);

bool ImGuiFreeType::BuildFontAtlas(ImFontAtlas* atlas, unsigned int extra_flags);

void ImGuiFreeType::SetAllocatorFunctions(void* (*alloc_func)(size_t sz, void* user_data), void (*free_func)(void* ptr, void* user_data), void* user_data);
#include "UI.hpp"
#include <imgui/imgui_freetype.h>
#include "rendering/imgui_sw.hpp"
#include <switch.h>

Framebuffer fb;

ImFont* Font25;
ImFont* Font35;

#define DEFAULT_WIN_WIDTH 1280
#define DEFAULT_WIN_HEIGHT 720

void UIInit() {
	auto win = nwindowGetDefault();

    Result rc = framebufferCreate(&fb, win, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT, PIXEL_FORMAT_RGBA_8888, 2);
	if (R_FAILED(rc))
		fatalSimple(rc);
    
    rc = framebufferMakeLinear(&fb);
	if (R_FAILED(rc))
		fatalSimple(rc);

	ImguiInitialize();

	/* ==Font init== */
	rc = plInitialize();
	if (R_FAILED(rc))
		fatalSimple(rc);
	
	PlFontData font;
    rc = plGetSharedFontByType(&font, PlSharedFontType_Standard);
	if (R_FAILED(rc))
		fatalSimple(rc);
	
	//ImFontAtlasFlags_NoPowerOfTwoHeight may be needed when/if we switch to hardware rendering
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight | ImFontAtlasFlags_NoMouseCursors;
	Font25 = io.Fonts->AddFontFromMemoryTTF((void*)font.address, font.size, 25.0f);
	Font35 = io.Fonts->AddFontFromMemoryTTF((void*)font.address, font.size, 35.0f);
	ImGuiFreeType::BuildFontAtlas(io.Fonts, 0u);
	
	plExit();

	imgui_sw::bind_imgui_painting();
}

void FrameStart() {
	ImGui::NewFrame();
}

void ClearFramebuffer()
{
	u32 *pixels = (u32*)framebufferBegin(&fb, nullptr);
	memset(pixels, 0, sizeof(u32) * DEFAULT_WIN_WIDTH * DEFAULT_WIN_HEIGHT);
	framebufferEnd(&fb);	
}

void FrameEnd() {	
	ImGui::Render();
	
	u32 *pixels = (u32*)framebufferBegin(&fb, nullptr);
	memset(pixels, 0, sizeof(u32) * DEFAULT_WIN_WIDTH * DEFAULT_WIN_HEIGHT);
	imgui_sw::paint_imgui(pixels, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT);
	framebufferEnd(&fb);	
}
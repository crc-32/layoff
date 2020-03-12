#include "UI.hpp"
#include "rendering/imgui_sw.hpp"
#include <switch.h>

Framebuffer fb;

ImFont* Font25;
ImFont* Font30;

#define DEFAULT_WIN_WIDTH 1280
#define DEFAULT_WIN_HEIGHT 720

void UIInit() {
	auto win = nwindowGetDefault();

    Result rc = framebufferCreate(&fb, win, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT, PIXEL_FORMAT_RGBA_8888, 2);
	if (R_FAILED(rc))
		fatalThrow(rc);
    
    rc = framebufferMakeLinear(&fb);
	if (R_FAILED(rc))
		fatalThrow(rc);

	ImguiInitialize();

	/* ==Font init== */
	rc = plInitialize();
	if (R_FAILED(rc))
		fatalThrow(rc);
	
	PlFontData font;
    rc = plGetSharedFontByType(&font, PlSharedFontType_Standard);
	if (R_FAILED(rc))
		fatalThrow(rc);
	
	//ImFontAtlasFlags_NoPowerOfTwoHeight may be needed when/if we switch to hardware rendering
	ImGuiIO& io = ImGui::GetIO();

	io.Fonts->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight | ImFontAtlasFlags_NoMouseCursors;

	ImFontConfig cfg{};
	cfg.FontDataOwnedByAtlas = false;

	Font25 = io.Fonts->AddFontFromMemoryTTF((void*)font.address, font.size, 25.0f, &cfg);
	Font30 = io.Fonts->AddFontFromMemoryTTF((void*)font.address, font.size, 30.0f, &cfg);

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
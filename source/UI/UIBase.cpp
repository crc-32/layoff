#include "UI.hpp"
#include <switch.h>
#include <cstdio>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "imgui_freetype.h"
#include "imgui_freetype.hpp"

using namespace std;

#define SDLLOG(eCode) { fprintf(stderr,"\nERR:\n%s", SDL_GetError()); fatalSimple(MAKERESULT(255, eCode));}

volatile int renderDirty = 0;

Gfx::Gfx()
{	
	width = 1280;
	height = 720;

	win = nwindowGetDefault();
	if(!win)
	{
		fatalSimple(MAKERESULT(255,120));
	}

	Result rc = framebufferCreate(&fb, win, width, height, PIXEL_FORMAT_RGBA_8888, 2);
	if (R_FAILED(rc))
		fatalSimple(rc);

	rc = framebufferMakeLinear(&fb);
	if (R_FAILED(rc))
		fatalSimple(rc);

	ViDisplay disp;
	rc = viGetDisplayVsyncEvent(&disp, &vsync);

	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.DisplaySize = ImVec2(width, height);

	rc = plInitialize();
	IM_ASSERT("plInitialize failed" && R_SUCCEEDED(rc));

	PlFontData font;
    rc = plGetSharedFontByType(&font, PlSharedFontType_Standard);
	IM_ASSERT("plGetSharedFontByType failed" && R_SUCCEEDED(rc));

	io.Fonts->AddFontFromMemoryTTF((void*)font.address, font.size, 25.0f);
	unsigned int flags = 0;
	ImGuiFreeType::BuildFontAtlas(io.Fonts, flags);

	imgui_sw::bind_imgui_painting(io);
	renderDirty = 4;
}

void Gfx::Render()
{
	if(renderDirty > 0)
	{
		eventWait(&vsync, 1e+6);
		u32 stride;
		u32 *pixels = (u32*)framebufferBegin(&fb, &stride);
		this->Clear(pixels);
		paint_imgui(pixels, width, height, sw_options);
		framebufferEnd(&fb);
		renderDirty--;
	}
}

void Gfx::Clear()
{
	u32 stride;
	void *pixels = framebufferBegin(&fb, &stride);
	memset(pixels, 0, sizeof(u32)*width*height);
	framebufferEnd(&fb);
}

void Gfx::Clear(u32 *pixels)
{
	memset(pixels, 0, sizeof(u32)*width*height);
}

void Gfx::Exit()
{
	framebufferClose(&fb);
	ImGui::DestroyContext();
}
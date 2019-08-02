#pragma once
#include <switch.h>
#include <vector>

#include "imgui.h"
#include "imgui_sw.hpp"

#define SCR_W 1280
#define SCR_H 720

//extern SDL_Window* sdl_win;
//extern SDL_Renderer* sdl_render;

//void SdlInit();
//void SdlExit();

class UiItem
{
public:
	virtual bool Draw() = 0;
	virtual ~UiItem() {};
};

class Gfx
{
	NWindow* win;
	Framebuffer fb;

	u32 *pixel_buffer;

	imgui_sw::SwOptions sw_options;

	u32 width;
	u32 height;
	Event vsync;
	public:
	Gfx();
	void Render();
	void Exit();
	void Clear();
	void Clear(u32 *pixels, u32 stride);
};
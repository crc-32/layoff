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

	std::vector<u32> pixel_buffer;

	imgui_sw::SwOptions sw_options;

	u32 width;
	u32 height;
	public:
	ImGuiIO io;
	Gfx();
	void StartRendering();
	void EndRendering();
	void Exit();
};
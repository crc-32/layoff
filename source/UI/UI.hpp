#pragma once
#include <switch.h>

#include "imgui.h"
#include "imgui_sdl.h"

#define SCR_W 1280
#define SCR_H 720

extern SDL_Window* sdl_win;
extern SDL_Renderer* sdl_render;

void SdlInit();
void SdlExit();

class UiItem
{
public:
	virtual bool Draw() = 0;
	virtual ~UiItem() {};
};
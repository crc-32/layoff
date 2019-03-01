#include "UI.hpp"
#include <switch.h>
#include <cstdio>

using namespace std;

SDL_Window* sdl_win;
SDL_Renderer* sdl_render;

extern void RemapErr();

#define SDLLOG(eCode) { fprintf(stderr,"\nERR:\n%s", SDL_GetError()); fatalSimple(MAKERESULT(666, eCode));}

void SdlInit()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		SDLLOG(1)
	
	sdl_win = SDL_CreateWindow("sdl2_gles2", 0, 0, 1280, 720, 0);
	if (!sdl_win)
		SDLLOG(2)
	
	sdl_render = SDL_CreateRenderer(sdl_win, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!sdl_render)
		SDLLOG(3)
	
	SDL_SetRenderTarget(sdl_render, NULL);
	
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("romfs:/opensans.ttf", 40);
	ImGuiSDL::Initialize(sdl_render, 1280, 720);
}

void SdlExit()
{	
	ImGuiSDL::Deinitialize();

	SDL_DestroyWindow(sdl_win);
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_Quit();

	ImGui::DestroyContext();
}
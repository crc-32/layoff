#include "UI.hpp"
#include <switch.h>
#include <cstdio>

using namespace std;

#define SDLLOG(eCode) { fprintf(stderr,"\nERR:\n%s", SDL_GetError()); fatalSimple(MAKERESULT(255, eCode));}

Gfx::Gfx()
{	
	width = 1280;
	height = 720;

	win = nwindowGetDefault();
	if(!win)
	{
		fatalSimple(MAKERESULT(255,120));
	}

	framebufferCreate(&fb, win, width, height, PIXEL_FORMAT_RGBA_8888, 2);

	framebufferMakeLinear(&fb);
	ImGui::CreateContext();
	this->io = ImGui::GetIO();
	imgui_sw::bind_imgui_painting(this->io);
}

void Gfx::StartRendering()
{
	pixel_buffer.clear();
	pixel_buffer.assign(width * height, 0);
	framebufferBegin(&fb, NULL);
}

void Gfx::EndRendering()
{
	paint_imgui(pixel_buffer.data(), width, height, sw_options);
	framebufferEnd(&fb);
}

void Gfx::Exit()
{
	framebufferClose(&fb);
	ImGui::DestroyContext();
}

/*void SdlInit()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		SDLLOG(1)
	
	sdl_win = SDL_CreateWindow(NULL, 0, 0, 640, 360, 0);
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
}*/
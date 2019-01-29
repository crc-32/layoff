#include "UI.hpp"
#include <switch.h>
#include <cstdio>

using namespace std;

SDL_Window* sdl_win;
SDL_Renderer* sdl_render;

/*
Usually the user application doesn't need to get the internal data of SDL but here we want to get the viDisplay instance from the driver so we declare this internal function that will return a void* to a struct defined by the implementation, exactly from this file:
https://github.com/devkitPro/SDL/blob/aa66a900d790d7a93ab7aa369dc5a264bebc2c57/src/video/switch/SDL_switchvideo.h#L34
This may break if this struct is changed in the source, it's unlikely as it just got updated with the new libnx apis
*/
extern "C"{ 
	extern DECLSPEC const char * SDLCALL SDL_GetDisplayDriverData(int displayIndex);
}

#define SDLLOG(eCode) { FILE * f = fopen("/sdllog.txt", "w"); if (!f) fatalSimple(MAKERESULT(999, eCode)); fprintf(f,"%s", SDL_GetError()); fclose(f); fatalSimple(MAKERESULT(666, eCode));}

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

	SDL_DisplayData *ddata = (SDL_DisplayData *)SDL_GetDisplayDriverData(0);
	SDL_WindowData *wdata = (SDL_WindowData *)sdl_win->driverdata;
	
	viSetDisplayAlpha(&ddata->viDisplay, 1.0f);
	u64 maxZ = 0;
	viGetDisplayMaximumZ(&ddata->viDisplay, &maxZ);	
    viSetLayerZ(&wdata->viLayer, maxZ);
	
	SDL_SetRenderTarget(sdl_render, NULL);
	
	IMG_Init( IMG_INIT_PNG | IMG_INIT_JPG);
	TTF_Init();	//This WILL fail as currently we're not replacing the romfs
}

void SdlExit()
{	
	IMG_Quit();
	TTF_Quit();
	
	SDL_Delay(10);
	SDL_DestroyWindow(sdl_win);

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_Quit();
}

#define LoadFont(num) TTF_OpenFont("romfs:/opensans.ttf", num);
TTF_Font *font20;
TTF_Font *font25;
TTF_Font *font30;
TTF_Font *font40;
void FontInit()
{
	font20 = LoadFont(20);
	font25 = LoadFont(25);
	font30 = LoadFont(30);
	font40 = LoadFont(40);
}

void FontExit()
{
	TTF_CloseFont(font20);
	TTF_CloseFont(font25);
	TTF_CloseFont(font30);
	TTF_CloseFont(font40);
}
#undef LoadFont









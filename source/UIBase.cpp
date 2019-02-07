#include "UI.hpp"
#include <switch.h>
#include <cstdio>

using namespace std;

SDL_Window* sdl_win;
SDL_Renderer* sdl_render;

extern void RemapErr();

#define SDLLOG(eCode) { fprintf(stderr,"\nERR:\n%s", SDL_GetError()); RemapErr(); fatalSimple(MAKERESULT(666, eCode));}

void SdlInit()
{
	fflush(stderr);
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		SDLLOG(1)
	
	sdl_win = SDL_CreateWindow("sdl2_gles2", 0, 0, 1280, 720, 0);
	if (!sdl_win)
		SDLLOG(2)
	
	sdl_render = SDL_CreateRenderer(sdl_win, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!sdl_render)
		SDLLOG(3)
	
	SDL_SetRenderTarget(sdl_render, NULL);
	IMG_Init( IMG_INIT_PNG | IMG_INIT_JPG);
	TTF_Init();
	FontInit();
}

void SdlExit()
{	
	FontExit();
	TTF_Quit();
	IMG_Quit();
	
	SDL_Delay(10);
	SDL_DestroyWindow(sdl_win);

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_Quit();
}

#define LoadFont(num) TTF_OpenFont("/opensans.ttf", num);
TTF_Font *font20;
TTF_Font *font25;
TTF_Font *font30;
TTF_Font *font40;
void FontInit()
{
	font20 = LoadFont(20);
	if (!font20)
		fatalSimple(MAKERESULT(666, 8));
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

LoadedImage OpenImage(const string &Path)
{	
	LoadedImage res = {0};
	SDL_Surface *surf = IMG_Load(Path.c_str());
	res.image = SDL_CreateTextureFromSurface(sdl_render, surf);
	res.Rect =  { 0, 0, surf->w, surf->h };
	SDL_FreeSurface(surf);	
	return res;
}

void FreeImage(LoadedImage &img)
{
	SDL_DestroyTexture(img.image);	
}

LoadedImage LoadImage(const vector<u8> &data)
{	
	auto rwOps = SDL_RWFromConstMem(reinterpret_cast<const char*>(data.data()),data.size());
	SDL_Surface *surf = IMG_Load_RW(rwOps,1);
	
	if(!surf) {
		printf("IMG_Load_RW: %s\n", IMG_GetError());
		fatalSimple(MAKERESULT(666, 10));
		return {};
	}
	
	LoadedImage res;
	res.image = SDL_CreateTextureFromSurface(sdl_render, surf);
	
	if (res.image == NULL) {
		printf("CreateTextureFromSurface failed: %s\n", SDL_GetError());
	}
	
	res.Rect =  { 0, 0, surf->w, surf->h };
	SDL_FreeSurface(surf);
	return res;
}








#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <switch.h>

#include <EGL/egl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#define SCR_W 1280
#define SCR_H 720

//I've recycled ~once again~ my graphics stuff to quickly have a base to work with, we can remove this as soon as we get sdl working

const SDL_Color WHITE = {0xff,0xff,0xff, 0xFF};
const SDL_Color BLACK = {0,0,0, 0xFF};

extern SDL_Window* sdl_win;
extern SDL_Renderer* sdl_render;

void SdlInit();
void SdlExit();

extern TTF_Font *font20;
extern TTF_Font *font25;
extern TTF_Font *font30;
extern TTF_Font *font40;

void FontInit();
void FontExit();

extern const SDL_Rect ScreenRect;

#define defProp(name,type) type Get ## name(); void Set ## name(type arg); 
class Label
{
	private:
		void RenderString();
		SDL_Texture* tex = NULL;
		std::string string = "";
		int wrap = -1;
		SDL_Color color = WHITE;
		SDL_Rect rect = {0,0,0,0};
		TTF_Font* font;
	public:
		SDL_Rect GetSize();
		
		defProp(String,std::string)
		defProp(Wrap,int)
		defProp(Color,SDL_Color)
		defProp(Font,TTF_Font*)
	
		Label(const std::string &str, SDL_Color _color, int _wrap, TTF_Font* fnt = font20);
		~Label();
		void Render(int X, int Y);
};
#undef defProp


//Internal sdl structs
typedef struct
{
    SDL_Surface *surface;
    int x_offset;
    int y_offset;
	ViLayer viLayer;
	NWindow nWindow;
	Framebuffer fb;
} SWITCH_WindowData;
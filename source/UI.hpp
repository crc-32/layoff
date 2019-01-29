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
typedef struct SDL_DisplayData
{
	ViDisplay viDisplay;
	EGLDisplay egl_display;
} SDL_DisplayData;

typedef struct SDL_DisplayModeData
{
} SDL_DisplayModeData;

typedef struct SDL_WindowData
{
	ViLayer viLayer;
	NWindow nWindow;
	EGLSurface egl_surface;
} SDL_WindowData;

//Use this struct just to get driverdata, some internal type pointers were replaced with void pointers  
struct SDL_Window 
{
	const void *magic;
	Uint32 id;
	char *title;
	SDL_Surface *icon;
	int x, y;
	int w, h;
	int min_w, min_h;
	int max_w, max_h;
	Uint32 flags;
	Uint32 last_fullscreen_flags;
	
	/* Stored position and size for windowed mode */
	SDL_Rect windowed;
	
	SDL_DisplayMode fullscreen_mode;
	
	float opacity;
	
	float brightness;
	Uint16 *gamma;
	Uint16 *saved_gamma;        /* (just offset into gamma) */
	
	SDL_Surface *surface;
	SDL_bool surface_valid;
	
	SDL_bool is_hiding;
	SDL_bool is_destroying;
	SDL_bool is_dropping;       /* drag/drop in progress, expecting SDL_SendDropComplete(). */
	
	void *shaper;
	
	SDL_HitTest hit_test;
	void *hit_test_data;
	
	void *data;
	
	void *driverdata;
	
	SDL_Window *prev;
	SDL_Window *next;
};
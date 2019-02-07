// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include the main libnx system header, for Switch development
#include "UI.hpp"
#include <switch.h>

extern "C" {
    extern u32 __start__;
    extern u32 __nx_applet_type;
    extern __attribute__((weak)) size_t __nx_heap_size;

    __attribute__((weak)) size_t __nx_heap_size = 0x4000000;
    u32 __nx_applet_type = AppletType_OverlayApplet;

    void __attribute__((weak)) __nx_win_init(void);
    void __attribute__((weak)) userAppInit(void);

    void __attribute__((weak)) __appInit(void)
    {
        Result rc;
        rc = smInitialize();
        if (R_FAILED(rc))
            fatalSimple(MAKERESULT(Module_Libnx, LibnxError_InitFail_SM));

        rc = appletInitialize();
        if (R_FAILED(rc))
            fatalSimple(MAKERESULT(Module_Libnx, LibnxError_InitFail_AM));

        rc = hidInitialize();
        if (R_FAILED(rc))
            fatalSimple(MAKERESULT(Module_Libnx, LibnxError_InitFail_HID));

        rc = plInitialize();
        if (R_FAILED(rc))
            fatalSimple(rc);

		rc = fsInitialize();
		if (R_FAILED(rc))
			fatalSimple(MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));

		fsdevMountSdmc();

        if (&userAppInit) userAppInit();
    }

    void __attribute__((weak)) userAppExit(void);
    void __attribute__((weak)) __nx_win_exit(void);

    void __attribute__((weak)) __appExit(void)
    {
        if (&userAppExit) userAppExit();
        if (&__nx_win_exit) __nx_win_exit();

        // Cleanup default services.
		fsdevUnmountAll();
		fsExit();
        appletExit();
        hidExit();
        smExit();
    }
}

//#define USE_REDIR_STDERR

//Not sure if needed, this will close the errlog file before calling fatal
void RemapErr() 
{
#ifdef USE_REDIR_STDERR
	freopen("/noerr.txt", "w", stderr);
#endif
}
 
// Main program entrypoint
int main(int argc, char* argv[])
{    
    svcSleepThread(5e+9);
	__nx_win_init(); 
	
#ifdef USE_REDIR_STDERR
	freopen("/errlog.txt", "w", stderr);
#endif

	romfsInit();	//For some reason romfs doesn't work anymore (?)
	FILE *font = fopen("romfs:/opensans.ttf","rb");
	
	SdlInit();
	SDL_SetRenderDrawBlendMode(sdl_render, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(sdl_render,0 ,0,0,0);
	
	Label lbl("",WHITE, -1, font30);
	Image img("/trollface.png");
	SDL_Rect bg {0,0, 600,100};
	u64 counter = 0;
    while (appletMainLoop())
    {		
		SDL_SetRenderDrawColor(sdl_render,0 ,0,0,0);
		SDL_RenderClear(sdl_render);
		lbl.SetString("Hello layout, Have a counter: " + std::to_string(counter++) + (font ? " - Romfs" : " - noRomfs"));
		SDL_SetRenderDrawColor(sdl_render,0 ,0,0,0x7F);
		SDL_RenderFillRect(sdl_render, &bg);
		lbl.Render(20,20);
		img.Render(757,359);
		SDL_RenderPresent(sdl_render);
    }
	SdlExit();
    return 0;
}

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
		
		rc = hidSysInitialize();
        if (R_FAILED(rc))
            fatalSimple(MAKERESULT(255, 11));

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
		hidSysExit();
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

Event homeMenuEvent = {0};

#include "demo/SdlEyes.hpp"
int EyesDemo()
{	
	hidSysExit();
	hidSysInitialize();
	hidSysEnableAppletToGetInput(true);	
	SdlEyes eyes(0,200);
	eyes.X = 1280/2 - eyes.TotalW / 2;
	eyes.Update(1280/2, 720);
    while (appletMainLoop())
    {	
		hidScanInput();
		
		SDL_SetRenderDrawColor(sdl_render,0 ,0,0,0);
		SDL_RenderClear(sdl_render);
		
		u32 touch_count = hidTouchCount();
		if (touch_count > 0)
		{
			touchPosition touch;
			hidTouchRead(&touch, 0);
			eyes.Update(touch.px, touch.py);			
		}		
		
		eyes.Render();
		
		SDL_RenderPresent(sdl_render);
		svcSleepThread(33333333); //lock to ~30 fps
		
		if (hidKeysDown(CONTROLLER_P1_AUTO) & KEY_PLUS)
			break;
    }	
	hidSysAcquireHomeButtonEventHandle(&homeMenuEvent); //re-aquire the home menu event as the hidSys service has been terminated.
	return 0;
}

int main(int argc, char* argv[])
{    
    svcSleepThread(5e+9);
	__nx_win_init(); 
	
#ifdef USE_REDIR_STDERR
	freopen("/errlog.txt", "w", stderr);
#endif

	romfsInit();	
	SdlInit();
	SDL_SetRenderDrawBlendMode(sdl_render, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(sdl_render,0 ,0,0,0);
	
	Label menuLbl("LAYOFF !\n\nPress A for home menu\nPress X for swEyes demo\nPress B to cancel.",WHITE, 950, font30);
	Image img("romfs:/trollface.png");	
	SDL_Rect bg {320, 100, 640,520};
	
	hidSysAcquireHomeButtonEventHandle(&homeMenuEvent);

RESET:
	SDL_SetRenderDrawColor(sdl_render,0 ,0,0,0);
	SDL_RenderClear(sdl_render);
	SDL_RenderPresent(sdl_render);
	
	appletEndToWatchShortHomeButtonMessage(); //Unlock input for the foreground app
	eventClear(&homeMenuEvent); //the event is usually set on boot.
	
	eventWait(&homeMenuEvent,U64_MAX);
	eventClear(&homeMenuEvent);
	appletBeginToWatchShortHomeButtonMessage(); //Lock input for the foreground app.
	
	u64 counter = 0;
    while (appletMainLoop())
    {		
		SDL_SetRenderDrawColor(sdl_render,0 ,0,0,0);
		SDL_RenderClear(sdl_render);
		SDL_SetRenderDrawColor(sdl_render,0 ,0,0,0x7F);
		SDL_RenderFillRect(sdl_render, &bg);
		menuLbl.Render(320 + 5,110);
		img.Render(320 , 570);
		SDL_RenderPresent(sdl_render);
		
		hidScanInput();
		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		if (kDown & KEY_A)
		{
			libappletRequestHomeMenu();			
			goto RESET;
		}
		else if (kDown & KEY_B)
		{	
			goto RESET;
		}
		else if (kDown & KEY_X)
		{
			appletEndToWatchShortHomeButtonMessage();
			EyesDemo();			
			appletBeginToWatchShortHomeButtonMessage();
			continue;
		}
		
		svcSleepThread(33333333); //lock to ~30 fps
    }
	SdlExit();
    return 0;
}

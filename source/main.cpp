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

        viInitialize(ViServiceType_Manager);
		rc = fsInitialize();
		if (R_FAILED(rc))
			fatalSimple(MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));

		fsdevMountSdmc();

        if (&__nx_win_init) __nx_win_init();
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

//Not sure if needed, this will close the errlog file before calling fatal
void RemapErr() { freopen("/noerr.txt", "w", stderr);}
 
// Main program entrypoint
int main(int argc, char* argv[])
{    
    svcSleepThread(5000000);
	freopen("/errlog.txt", "w", stderr);
	romfsInit();	
	SdlInit();
	SDL_SetRenderDrawBlendMode(sdl_render, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(sdl_render,0 ,0,0,0x7F);
	
	Label lbl("",WHITE, -1, font30);
	SDL_Rect bg {0,0, 600,100};
	u64 counter = 0;
    while (appletMainLoop())
    {
		lbl.SetString("Hello layout, Have a counter: " + std::to_string(counter++));
		SDL_RenderFillRect(sdl_render, &bg);
		lbl.Render(20,20);
		SDL_RenderPresent(sdl_render);
    }
	SdlExit();
    return 0;
}

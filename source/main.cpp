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

        //viInitialize(ViServiceType_Manager);
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
// Main program entrypoint
int main(int argc, char* argv[])
{    
    svcSleepThread(1000000);
	SdlInit();
	//SDL_SetRenderDrawBlendMode(sdl_render, SDL_BLENDMODE_BLEND); //Currently disabled to rule out possible issues
	SDL_SetRenderDrawColor(sdl_render,0xff ,0xff,0xff,0x7F); //Drawing white to test on the black background of the nro
	
	//Label lbl("Hello",WHITE, -1, font30); //Font loading WILL fail as currently we're not replacing the romfs
	SDL_Rect bg {0,0, 200,100};
    while (appletMainLoop())
    {
		SDL_RenderFillRect(sdl_render, &bg);
		//lbl.Render(20,20);
		SDL_RenderPresent(sdl_render);		
    }
	SdlExit();
    return 0;
}

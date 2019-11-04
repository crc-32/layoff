// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <switch.h>
#include "UI/UI.hpp"

#define INNER_HEAP_SIZE 0x2000000*2

extern "C" {
    u32 __nx_applet_type = AppletType_OverlayApplet;
    size_t nx_inner_heap_size = INNER_HEAP_SIZE;
    char   nx_inner_heap[INNER_HEAP_SIZE];
	
	extern void __nx_win_init(void);
	extern void __nx_win_exit(void);

    void __libnx_initheap(void)
    {
        void*  addr = nx_inner_heap;
        size_t size = nx_inner_heap_size;

        extern char* fake_heap_start;
        extern char* fake_heap_end;

        fake_heap_start = (char*)addr;
        fake_heap_end   = (char*)addr + size;
    }

    void __attribute__((weak)) __appInit(void) {
        Result rc;

        rc = smInitialize();
        if (R_FAILED(rc))
            fatalSimple(MAKERESULT(Module_Libnx, LibnxError_InitFail_SM));

        rc = appletInitialize();
        if (R_FAILED(rc))
            fatalSimple(MAKERESULT(Module_Libnx, LibnxError_InitFail_AM));

        rc = npnsInitialize();
        if (R_FAILED(rc))
            fatalSimple(MAKERESULT(255, 10));

        rc = hidInitialize();
	    if (R_FAILED(rc))
		    fatalSimple(MAKERESULT(Module_Libnx, LibnxError_InitFail_HID));

        rc = hidsysInitialize();
            if (R_FAILED(rc))
                fatalSimple(MAKERESULT(255, 11));

        rc = fsInitialize();
        if (R_FAILED(rc))
            fatalSimple(MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));

        fsdevMountSdmc();
    }

    void __attribute__((weak)) userAppExit(void);

    void __attribute__((weak)) __appExit(void) {
        fsdevUnmountAll();
        fsExit();
        hidsysExit();
        appletExit();
        hidExit();
        smExit();
    }
}

bool HomeLongPressed = false;
bool HomePressed = false;
bool PowerPressed = false;
bool ActiveMode = false;

bool MessageLoop(void) {
    u32 msg = 0;
    if (R_FAILED(appletGetMessage(&msg))) return true;
	
	if (msg == 0x17)
		PowerPressed = true;
	else if (msg == 0x15)
		HomeLongPressed = true;
	else if (msg == 0x14)
		HomePressed = true;
	else return appletProcessMessage(msg);

	return true;
}

void SwitchToActiveMode()
{	
	appletBeginToWatchShortHomeButtonMessage();
	ActiveMode = true;
}

void SwitchToPassiveMode()
{
	appletEndToWatchShortHomeButtonMessage();
	//Workaround for a glitch (or is it on purpose ?) that won't let the overlay applet get inputs without calling appletBeginToWatchShortHomeButtonMessage
	hidsysExit();
	hidsysInitialize();
	hidsysEnableAppletToGetInput(true); 
	ActiveMode = false;
}

bool IdleLoop() {
    HomeLongPressed = false;
    HomePressed = false;
    PowerPressed = false;
    while (MessageLoop())
    {
        if (PowerPressed || HomeLongPressed)
            return true;
        svcSleepThread(5e+7);
    }
    return false;
}

bool ActiveLoop() {
    while (MessageLoop())
    {
        UIStart();
        UIUpdate();
        if (HomeLongPressed || HomePressed)
            return true;
    }
    return false;
}

int main(int argc, char* argv[]) {
    svcSleepThread(5e+9);
    __nx_win_init();

    romfsInit();
    ovlnInitialize();

    SwitchToPassiveMode();
    UIInit(nwindowGetDefault());
    while (true) {
        /*if(!IdleLoop()) break;
        HomeLongPressed = false;
        HomePressed = false;
        PowerPressed = false;
        SwitchToActiveMode();

        if(!ActiveLoop()) break;*/
        UIStart();
        ImGui::Text("Hello");
        UIUpdate();
        svcSleepThread(5e+5);
    }
    ovlnExit();
    npnsExit();
	__nx_win_exit();
    return 0;
}

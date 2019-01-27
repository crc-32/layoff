// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include the main libnx system header, for Switch development
#include "drawing.hpp"
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

        viInitialize(ViServiceType_Manager);

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
        appletExit();
        hidExit();
        smExit();
    }
}
// Main program entrypoint
int main(int argc, char* argv[])
{
    Drawing *drawing = new Drawing();
    svcSleepThread(1000000);
    drawing->Setup();
    while (appletMainLoop())
    {
        drawing->Test();
        svcSleepThread(10000);
    }
    drawing->Exit();
    return 0;
}

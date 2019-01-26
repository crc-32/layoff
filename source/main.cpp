// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include the main libnx system header, for Switch development
#include "drawing.hpp"
#include <switch.h>
#define INNER_HEAP_SIZE 0x80000

extern "C" {
    extern u32 __start__;
    extern u32 __nx_applet_type = AppletType_OverlayApplet;


    size_t nx_inner_heap_size = INNER_HEAP_SIZE;
    char   nx_inner_heap[INNER_HEAP_SIZE];

    void __libnx_initheap(void)
    {
        void*  addr = nx_inner_heap;
        size_t size = nx_inner_heap_size;

        // Newlib
        extern char* fake_heap_start;
        extern char* fake_heap_end;

        fake_heap_start = (char*)addr;
        fake_heap_end   = (char*)addr + size;
    }

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
    bool draw = false;
    Drawing *drawing = new Drawing();
    drawing->Setup();
    while (appletMainLoop())
    {
        hidScanInput();
        u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
        if((kHeld & KEY_PLUS & KEY_MINUS)) {
            draw = !draw;
        }
        if(draw) {
            drawing->Test();
        }
    }
    drawing->Exit();
    return 0;
}

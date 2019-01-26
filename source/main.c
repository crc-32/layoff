// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include the main libnx system header, for Switch development
#include <switch.h>

u32 __nx_applet_type = AppletType_OverlayApplet;

// Main program entrypoint
int main(int argc, char* argv[])
{
    fatalSimple(MAKERESULT(360, 1));

    while (appletMainLoop())
    {
        svcSleepThread(10000000);
    }
    return 0;
}

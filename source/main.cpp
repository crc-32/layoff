
#if !defined(__SWITCH__)
#define __attribute__(x)
#endif

// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>

#include <switch.h>
#include "UI/UI.hpp"
#include "utils.hpp"

#include "UI/sidebar/Sidebar.hpp"
#include "UI/PowerWindow.hpp"
#include "ConsoleStatus.hpp"
#include "Config.hpp"
#include "NotificationManager.hpp"

using namespace layoff;

extern "C" {
    u32 __nx_applet_type = AppletType_OverlayApplet;
	
    __attribute__((weak)) size_t __nx_heap_size = 30 * 1024 * 1024;
	
	extern void __nx_win_init(void);
	extern void __nx_win_exit(void);

    void __attribute__((weak)) __appInit(void) {
        Result rc;

        rc = smInitialize();
        if (R_FAILED(rc))
            fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_SM));

        rc = appletInitialize();
        if (R_FAILED(rc))
            fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_AM));

        rc = hidInitialize();
	    if (R_FAILED(rc))
		    fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_HID));

        rc = hidsysInitialize();
            if (R_FAILED(rc))
                fatalThrow(MAKERESULT(255, 11));

        rc = fsInitialize();
        if (R_FAILED(rc))
            fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));

        fsdevMountSdmc();
    }

    void __attribute__((weak)) __appExit(void) {
        fsdevUnmountAll();
        fsExit();
        hidsysExit();
        appletExit();
        hidExit();
        smExit();
    }
}

u64 kHeld, kDown;
NotificationManager *layoff::nman = nullptr;
static inline void ImguiBindInputs()
{	
	ImGuiIO &io = ImGui::GetIO();
	
	hidScanInput();
	kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
	kDown = hidKeysDown(CONTROLLER_P1_AUTO);
	
	u32 touch_count = hidTouchCount();
	if (touch_count == 1)
	{
		touchPosition touch;
		hidTouchRead(&touch, 0);
		io.MousePos = ImVec2(touch.px, touch.py);
		io.MouseDown[0] = true;
	}       
	else io.MouseDown[0] = false;
	
	io.NavInputs[ImGuiNavInput_DpadDown] = kHeld & KEY_DOWN;
	io.NavInputs[ImGuiNavInput_DpadUp] = kHeld & KEY_UP;
	io.NavInputs[ImGuiNavInput_DpadLeft] = kHeld & KEY_LEFT;
	io.NavInputs[ImGuiNavInput_DpadRight] = kHeld & KEY_RIGHT;

	io.NavInputs[ImGuiNavInput_Activate] = kHeld & KEY_A;
	io.NavInputs[ImGuiNavInput_Cancel] = kHeld & KEY_B;
	io.NavInputs[ImGuiNavInput_Menu] = kHeld & KEY_X;
	io.NavInputs[ImGuiNavInput_FocusNext] = kHeld & (KEY_ZR | KEY_R);
	io.NavInputs[ImGuiNavInput_FocusPrev] = kHeld & (KEY_ZL | KEY_L);
}

static bool HomeLongPressed = false;
static bool HomePressed = false;
static bool PowerPressed = false;
static OverlayMode mode;

static void ClearEvents()
{	
    HomeLongPressed = false;
    HomePressed = false;
    PowerPressed = false;
}

//Getters defined in utils.hpp
bool layoff::IsHomeLongPressed() {return HomeLongPressed;}
bool layoff::IsHomePressed() {return HomePressed;}
bool layoff::IsPowerPressed() {return PowerPressed;}
OverlayMode layoff::GetCurrentMode() {return mode;}

static bool MessageLoop(void) {
    u32 msg = 0;
    if (R_FAILED(appletGetMessage(&msg))) return true;
	
	if (msg == 0x17)
		PowerPressed = true;
	else if (msg == 0x15)
		HomeLongPressed = true;
	else if (msg == 0x14)
		HomePressed = true;
	else		
	{
		PrintLn("Unknown msg: " + std::to_string(msg));
		return appletProcessMessage(msg);
	}

	return true;
}

// TODO
/*static void NotifThread(void* _arg) {
	Result rc = ovlnInitialize();
	if (R_FAILED(rc)) {
		ovlnExit();
		fatalThrow(rc);
	}

	Event notEv;
	rc = ovlnIReceiverGetEvent(&notEv);
	if (R_FAILED(rc)) {
		fatalThrow(rc);
	}
	while(true) {
		if (!R_FAILED(eventWait(&notEv, 1000000))) {
			IReceiverNotification notif;
			ovlnIReceiverGetNotification(&notif);
			std::stringstream nlog;
			nlog << "SysNotif: " << std::hex << notif.type;
			PrintLn(nlog.str());
		}else{
			break;
		}
	}
	fatalThrow(MAKERESULT(255, 10));
}*/

void layoff::SwitchToActiveMode()
{	
	if (mode == OverlayMode::Active) return;
	
	appletBeginToWatchShortHomeButtonMessage();
	mode = OverlayMode::Active;
}

void layoff::SwitchToPassiveMode()
{
	if (mode == OverlayMode::Passive) return;
	
	appletEndToWatchShortHomeButtonMessage();
	//Workaround for a glitch (or is it on purpose ?) that won't let the overlay applet get inputs without calling appletBeginToWatchShortHomeButtonMessage
	hidsysExit();
	hidsysInitialize();
	hidsysEnableAppletToGetInput(true); 
	mode = OverlayMode::Passive;
}

//Other components shouldn't switch to idle mode on their own
static void SwitchToIdleMode()
{
	if (mode == OverlayMode::Idle) return;
	
	if (mode == OverlayMode::Active) 
		appletEndToWatchShortHomeButtonMessage();
	
	mode = OverlayMode::Idle;
}

static UI::Sidebar mainWindow;
static UI::PowerWindow powerWindow;
static UI::WinPtr foregroundWin = nullptr;

#if LAYOFF_LOGGING
#include "UI/LogWindow.hpp"
static UI::LogWindow logWin;
#endif

//In the idle loop layoff only checks for events, when the idle loops breaks the active loop starts
static bool IdleLoop() {
	ClearFramebuffer();
	ClearEvents();
	
	//Close all the current windows 
	mainWindow.Visible = false;
	powerWindow.Visible = false;
	//The foreground window may want to persist
	if (foregroundWin)
		foregroundWin->RequestClose();
    
	SwitchToIdleMode();
	while (MessageLoop())
    {
        if (PowerPressed || HomeLongPressed)
		{
			powerWindow.Visible = PowerPressed;
			mainWindow.Visible = HomeLongPressed;
            return true;
		}
		FrameStart();
		nman->Update();
		FrameEnd();
        svcSleepThread(3e+8); // 3/10 of a second
    }
    return false;
}

//The active loop will draw either the power menu, sidebar or the active window.
static bool ActiveLoop() {
	ClearEvents();
    while (MessageLoop())
    {					
		console::UpdateStatus();
		
		ImguiBindInputs();		
        
		bool AnyWindowRendered = false;
		FrameStart();
		nman->Update();
		if (AnyWindowRendered = powerWindow.ShouldRender())
			powerWindow.Update();
		//The foreground window has to come before the sidebar as it can optionally stay open (but not do any process) when switching to idle mode
		else if (foregroundWin && (AnyWindowRendered = foregroundWin->ShouldRender()))
				foregroundWin->Update();
		else if (AnyWindowRendered = mainWindow.ShouldRender())
		{
			if (mode == OverlayMode::Idle)
				SwitchToActiveMode();	
			mainWindow.Update();
		}
		
		#if LAYOFF_LOGGING
			logWin.Update();
		#endif
		
        FrameEnd();		
        
		if (!AnyWindowRendered || HomeLongPressed || HomePressed)
			return true;
		
		svcSleepThread(1e+9 / 25); //25 fps-ish
	}
    return false;
}

#include "IPC/IPCThread.hpp"

int main(int argc, char* argv[]) {
    svcSleepThread(30e+9);
    __nx_win_init();

    romfsInit();
	if(!config::ConfigInit())
	{
		#ifdef LAYOFF_LOGGING
		PrintLn("ERR: Couldn't read layoff ini, using defaults");
		#endif
	}
	
	timeInitialize();
	psmInitialize();
	npnsInitialize();
	nifmInitialize(NifmServiceType_System);
	lblInitialize();

    SwitchToIdleMode();
    UIInit();

	nman = new NotificationManager();
	IPC::LaunchThread();

	//TODO
	/*Thread nThread;
	threadCreate(&nThread, NotifThread, NULL, NULL, 0x2000, 0x2D, -2);
	threadStart(&nThread);*/

    while (true)
	{		
        if(!IdleLoop()) break;
        SwitchToActiveMode();
        if(!ActiveLoop()) break;
    }

	lblExit();
	nifmExit();
    npnsExit();
	psmExit();
	timeExit();
	romfsExit();	
	__nx_win_exit();
    return 0;
}

// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>

#include <switch.h>
#include "utils.hpp"
#include "Config.hpp"
#include "ConsoleStatus.hpp"

#include "UI/UI.hpp"
#include "UI/sidebar/Sidebar.hpp"
#include "UI/PowerWindow.hpp"
#include "UI/NotificationWindow.hpp"
#include "UI/VolumeWindow.hpp"

#include "IPC/GeneralChannel.hpp"
#include "IPC/ServiceWrappers/npns.h"
#include "IPC/ServiceWrappers/ovln.h"

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

		rc = romfsInit();
		if (R_FAILED(rc))
			fatalThrow(MAKERESULT(255,12));

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
static UI::NotificationWindow notifWin;
static UI::VolumeWindow volumeWin;

#if LAYOFF_LOGGING
#include "debug/DebugControls.hpp"
#endif

static void NotifThread(void* _arg) {
	Result rc = ovlnInitialize();
	if (R_FAILED(rc)) {
		PrintLn("ovlnInit: " + R_STRING(rc));
		return;
	}

	Event notEv;
	rc = ovlnIReceiverGetReceiveEventHandle(&notEv);
	if (R_FAILED(rc)) {
		ovlnExit();
		PrintLn("ovlnEvent: " + R_STRING(rc));
		return;
	}
	while (true) {
		rc = eventWait(&notEv, UINT64_MAX);

		if (R_FAILED(rc))
		{
			PrintLn("ovln eventWait failed");
			break;
		}
		else
		{
			OvlnNotificationWithTick notif;

			if (R_FAILED(ovlnIReceiverReceiveWithTick(&notif)))
				continue;

			switch (notif.Type)
			{
			case OvlnNotificationType_Battery:
				console::RequestStatusUpdate();
				break;
			case OvlnNotificationType_LowBat:
				notif::PushSimple("Low battery !", "Layoff");
				break;
			case OvlnNotificationType_Volume:
				volumeWin.Signal(notif.Payload[0]);
				break;
			case OvlnNotificationType_Screenshot:
				notif::PushSimple("Screenshot saved !", "Layoff");
				break;
			case OvlnNotificationType_ScreenshotFail:
				notif::PushSimple("Couldn't save the screenshot.", "Layoff");
				break;
			case OvlnNotificationType_Video:
				notif::PushSimple("Recording saved !", "Layoff");
				break;
			case OvlnNotificationType_VideoFail:
				notif::PushSimple("Recording saved !", "Layoff");
				break;
			default:
				PrintLn("Got unknown notification " + std::to_string(notif.Type));
				PrintHex((u8*)&notif, sizeof(notif));
			}			
		}
	}

	ovlnExit();
	PrintLn("Exiting ovln notif thread");
	return;
}

//In the idle loop layoff only checks for events, when the idle loops breaks the active loop starts
static bool IdleLoop() {
	ClearEvents();
	
	//Close all the current windows 
	mainWindow.Visible = false;
	powerWindow.Visible = false;
	//The foreground window may want to persist
	if (foregroundWin)
		foregroundWin->RequestClose();
    
	SwitchToIdleMode();
	//As this is likely being called after the active loop we assume we have to clear the framebuffer
	bool previousFrameRendered = true;
	while (MessageLoop())
    {
        if (PowerPressed || HomeLongPressed)
		{
			powerWindow.Visible = PowerPressed;
			mainWindow.Visible = HomeLongPressed;
            return true;
		}

		const bool notif = notifWin.ShouldRender();
		const bool volume = volumeWin.ShouldRender();
		const bool shouldRender = notif || volume;
		const bool userCanInteract = volume;

		if (shouldRender)
		{
			FrameStart();
			if (volume)
				volumeWin.Update();
			if (notif)
				notifWin.Update(volume);
			FrameEnd();
			previousFrameRendered = shouldRender;
		}
		else if (previousFrameRendered)
		{
			//If we're leaving the active loop or just finished rendering
			ClearFramebuffer();
			previousFrameRendered = false;
		}
		
		svcSleepThread(userCanInteract ? (1e+9 / 15) : 3e+8); // ~15 FPS if we're rendering something the user can interact with, 1/3 of a sec otherwise
    }
    return false;
}

//The active loop will draw either the power menu, sidebar or the active window.
static bool ActiveLoop() {
	ClearEvents();
	console::RequestStatusUpdate();
	IPC::qlaunch::SignalOverlayOpened();
	SwitchToActiveMode();
	while (MessageLoop())
    {					
		console::UpdateStatus();
		
		ImguiBindInputs();		
        
		bool AnyWindowRendered = false;
		FrameStart();

		if ((AnyWindowRendered = powerWindow.ShouldRender()))
			powerWindow.Update();
		//The foreground window has to come before the sidebar as it can optionally stay open when switching to idle mode (but update it's not called)
		else if (foregroundWin && (AnyWindowRendered = foregroundWin->ShouldRender()))
				foregroundWin->Update();
		else if ((AnyWindowRendered = mainWindow.ShouldRender()))
			mainWindow.Update();

		//These are shown as overlay and don't need to prevent transitioning to the idle loop, so don't set AnyWindowRendered
		bool volume = volumeWin.ShouldRender();
		if (volume)
			volumeWin.Update();
		if (notifWin.ShouldRender())
			notifWin.Update(volume);

		#if LAYOFF_LOGGING
			debug::Instance.Update();
		#endif
		
        FrameEnd();		
        
		if (!AnyWindowRendered || HomeLongPressed || HomePressed)
		{
			IPC::qlaunch::SignalOverlayClosed();
			return true;
		}
		
		svcSleepThread(1e+9 / 25); //25 fps-ish
	}
    return false;
}

#include "IPC/IPCThread.hpp"
#include <time.h>

int main(int argc, char* argv[]) {
	//Waiting too little here breaks the power button messages, at least on 9.1 
    svcSleepThread(15e+9);
    __nx_win_init();

	if(!config::ConfigInit())
		PrintLn("ERR: Couldn't read layoff ini, using defaults");
	
	timeInitialize();
	psmInitialize();
	npnsInitialize();
	nifmInitialize(NifmServiceType_System);
	lblInitialize();

    SwitchToIdleMode();
    UIInit();

	notif::Initialize();
	IPC::LaunchThread();

	Thread nThread;
	threadCreate(&nThread, NotifThread, NULL, NULL, 0x2000, 0x2D, -2);
	threadStart(&nThread);
	srand(time(NULL));

    while (true)
	{		
        if(!IdleLoop()) break;
        if(!ActiveLoop()) break;
    }

	IPC::RequestAndWaitExit();
	lblExit();
	nifmExit();
    npnsExit();
	psmExit();
	timeExit();
	romfsExit();	
	__nx_win_exit();
    return 0;
}

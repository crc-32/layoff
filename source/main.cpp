// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>

#include <switch.h>
#include "UI/UI.hpp"
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

ViDisplay disp;
ViLayer layer;
NWindow win;

extern "C" {
	
    //__attribute__((weak)) size_t __nx_heap_size = 30 * 1024 * 1024;

	#define INNER_HEAP_SIZE 40 * 1024 * 1024
	size_t nx_inner_heap_size = INNER_HEAP_SIZE;
	char   nx_inner_heap[INNER_HEAP_SIZE];

	u32 __nx_applet_type = AppletType_OverlayApplet;

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

	void __nx_win_init(void)
	{
		Result rc;
		rc = viInitialize(ViServiceType_Default);
		if (R_SUCCEEDED(rc)) {
			rc = viOpenDefaultDisplay(&disp);
			if (R_SUCCEEDED(rc)) {
				rc = viCreateLayer(&disp, &layer);
				if (R_SUCCEEDED(rc)) {
					rc = viSetLayerScalingMode(&layer, ViScalingMode_FitToLayer);
					viSetLayerZ(&layer, 100);
					if (R_SUCCEEDED(rc)) {
						rc = nwindowCreateFromLayer(&win, &layer);
						if (R_SUCCEEDED(rc))
							nwindowSetDimensions(&win, 1280, 720);
					}
					if (R_FAILED(rc))
						viCloseLayer(&layer);
				}
				if (R_FAILED(rc))
					viCloseDisplay(&disp);
			}
			if (R_FAILED(rc))
				viExit();
		}
		if (R_FAILED(rc))
			fatalThrow(MAKERESULT(Module_Libnx, LibnxError_BadGfxInit));
	}

	void __nx_win_exit(void)
	{
		nwindowClose(&win);
		viCloseLayer(&layer);
		viCloseDisplay(&disp);
		viExit();
	}

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

Result capsscCaptureForDebug(void *buffer, size_t buffer_size, u64 *size) {
	Service capssc;
	smGetService(&capssc, "caps:sc");
    struct {
        u32 a;
        u64 b;
    } in = {0, 10000000000};
    return serviceDispatchInOut(&capssc, 1204, in, *size,
        .buffer_attrs = {SfBufferAttr_HipcMapTransferAllowsNonSecure | SfBufferAttr_HipcMapAlias | SfBufferAttr_Out},
        .buffers = { { buffer, buffer_size } },
    );
	serviceClose(&capssc);
}

int ssC = 0;
char buf[4*1280*720];
void overlayScreenshot() {
	size_t size;
	std::stringstream path;
	path << "sdmc:/ovlSS_" << ssC << ".raw";
	ssC++;
	Result rc = capsscCaptureForDebug(&buf, sizeof(buf), &size);
	if (R_FAILED(rc)) {
		std::stringstream err;
		err << "Failed to take overlay screenshot: " << R_MODULE(rc) << "-" << R_DESCRIPTION(rc);
		PrintLn(err.str());
		return;
	}else{
		PrintLn("Took overlay screenshot");
	}
	FILE* f = fopen(path.str().c_str(),"w");
	fwrite(buf, size, 1, f);
	fclose(f);
}
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
	if (kDown & KEY_L && kDown & KEY_R) {
		overlayScreenshot();
	}else {
		io.NavInputs[ImGuiNavInput_FocusNext] = kHeld & (KEY_ZR | KEY_R);
		io.NavInputs[ImGuiNavInput_FocusPrev] = kHeld & (KEY_ZL | KEY_L);
	}
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
				PrintHex((u8*)&notif, sizeof(notif)); //TODO remove this
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
	UI::SlowMode();
	
	//Close all the current windows 
	mainWindow.Visible = false;
	powerWindow.Visible = false;
	//The foreground window may want to persist
	if (foregroundWin)
		foregroundWin->RequestClose();
    
	SwitchToIdleMode();
	//As this is likely being called after the active loop we assume we have to clear the framebuffer
	while (MessageLoop())
    {
        if (PowerPressed || HomeLongPressed)
		{
			powerWindow.Visible = PowerPressed;
			mainWindow.Visible = HomeLongPressed;
            return true;
		}
		FrameStart();

		const bool notif = notifWin.ShouldRender();
		const bool volume = volumeWin.ShouldRender();
		const bool shouldRender = notif || volume;
		const bool userCanInteract = volume;

		if (shouldRender)
		{
			if (volume)
				volumeWin.Update();
			if (notif)
				notifWin.Update(volume);
		}
		FrameEnd();
    }
    return false;
}

//The active loop will draw either the power menu, sidebar or the active window.
static bool ActiveLoop() {
	ClearEvents();
	UI::FastMode();
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
	}
    return false;
}

#include "IPC/IPCThread.hpp"
#include <time.h>

int main(int argc, char* argv[]) {
    svcSleepThread(15e+9);
    __nx_win_init();

    romfsInit();
	if(!config::ConfigInit())
		PrintLn("ERR: Couldn't read layoff ini, using defaults");
	
	timeInitialize();
	psmInitialize();
	npnsInitialize();
	nifmInitialize(NifmServiceType_System);
	lblInitialize();

    SwitchToIdleMode();
    UIInit(&win);

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
	plExit();

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

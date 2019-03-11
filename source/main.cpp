#include <cstdlib>
#include <iostream>
#include <string>
#include "UI/UI.hpp"
#include <switch.h>
#include <time.h>
#include "setsys.hpp"
#include "NotificationManager.hpp"
#include "Notification.hpp"

#define EVENT_FIRED(x) R_SUCCEEDED(eventWait(x,0))
#define EVENT_NOT_FIRED(x) R_FAILED(eventWait(x,0))

using namespace std;

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
		
		rc = hidsysInitialize();
		if (R_FAILED(rc))
			fatalSimple(MAKERESULT(255, 11));

		rc = plInitialize();
		if (R_FAILED(rc))
			fatalSimple(rc);

		rc = psmInitialize();
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

		fsdevUnmountAll();
		fsExit();
		appletExit();
		hidsysExit();
		psmExit();
		hidExit();
		smExit();
	}
}

#include "screenConsole.hpp"
ScreenConsole *console = nullptr;
NotificationManager *ntm = nullptr;

bool ActiveMode;
//Get inputs while blocking the foreground app, the long press home button is not detected in this mode
void SwitchToActiveMode()
{	
	appletBeginToWatchShortHomeButtonMessage();
	ActiveMode = true;
	console->Print("[Input active mode]\n");
}

//Get inputs without blocking the foreground app
void SwitchToPassiveMode()
{
	appletEndToWatchShortHomeButtonMessage();
	//Workaround for a glitch (or is it on purpose ?) that won't let the overlay applet get inputs without calling appletBeginToWatchShortHomeButtonMessage
	hidsysExit();
	hidsysInitialize();
	hidsysEnableAppletToGetInput(true); 
	ActiveMode = false;
	console->Print("[Input passive mode]\n");
}
u32 batteryPercentage = 0;
u64 ltimestamp;

//Events
bool HomeLongPressed = false;
bool HomePressed = false;
bool PowerPressed = false;
Event *notif = nullptr;
//Settings
bool IsWirelessEnabled = false;
bool OverlayAppletMainLoop(void) {
    u32 msg = 0;
    if (R_FAILED(appletGetMessage(&msg))) return true;
	
	if (console)
		console->Print("Received message: " + to_string(msg) + "\n");
	
	if (msg == 0x17)
		PowerPressed = true;
	else if (msg == 0x15)
		HomeLongPressed = true;
	else if (msg == 0x14)
		HomePressed = true;
	else return appletProcessMessage(msg);

	return true;
}

void ImguiBindInputs(ImGuiIO& io)
{
	hidScanInput();
	u32 touch_count = hidTouchCount();
	if (touch_count == 1)
	{
		touchPosition touch;
		hidTouchRead(&touch, 0);
		io.MousePos = ImVec2(touch.px, touch.py);
		io.MouseDown[0] = true;
	}       
	else io.MouseDown[0] = false;
	
	u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
	
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
Texture *statusTexTarget;
void StatusDisplay()
{
	//Battery
	std::string ptext = std::to_string(batteryPercentage) + "%%";
	ImVec2 ptextSize = ImGui::CalcTextSize(ptext.c_str());
	ImGuiStyle cStyle = ImGui::GetStyle();
	int bOffsetY = ptextSize.y/2;
    SDL_Rect Battery =  {450,bOffsetY-16,32,16};
    SDL_Rect BatteryT = {450+32,(bOffsetY-16)+4,4,8};
	SDL_SetRenderTarget(sdl_render, statusTexTarget->Source);
	ImVec4 bgCol = cStyle.Colors[ImGuiCol_WindowBg];
	ImVec4 fgCol = cStyle.Colors[ImGuiCol_Text];

	SDL_SetRenderDrawColor(sdl_render,  //ImVec4 to seperate RGBA
		max(0, min(255, (int)floor(bgCol.x * 256.0))),
		max(0, min(255, (int)floor(bgCol.y * 256.0))),
		max(0, min(255, (int)floor(bgCol.z * 256.0))),
		max(0, min(255, (int)floor(bgCol.w * 256.0))));
	SDL_RenderClear(sdl_render);

    SDL_SetRenderDrawColor(sdl_render,
		max(0, min(255, (int)floor(fgCol.x * 256.0))),
		max(0, min(255, (int)floor(fgCol.y * 256.0))),
		max(0, min(255, (int)floor(fgCol.z * 256.0))),
		max(0, min(255, (int)floor(fgCol.w * 256.0))));
    SDL_RenderDrawRect(sdl_render, &Battery);
    SDL_RenderFillRect(sdl_render, &BatteryT);
    SDL_Rect BatteryP = {450+2,(bOffsetY-16)+2,static_cast<int>(nearbyint(((double)batteryPercentage)*28/100)),12};
	if(batteryPercentage <= 15) {
		SDL_SetRenderDrawColor(sdl_render, 255, 0, 0, 255);
	}
    SDL_RenderFillRect(sdl_render, &BatteryP);

	//Back to rendering for window
	SDL_SetRenderTarget(sdl_render, NULL);
    ImGui::Image(statusTexTarget, ImVec2(512, 32));
	//Battery %
	ImGui::SetCursorPos(ImVec2(475-(ptextSize.x), 0));
	ImGui::Text(ptext.c_str());
	ImGui::Spacing();
}
#include "demo/SdlEyes.hpp"
#include "demo/Calc.hpp"
#include "demo/GameDemo.hpp"
SdlEyes *demoEyes = nullptr;
DemoCalc *demoCalc = nullptr;
DemoGame *demoGame = nullptr;

void LayoffMainWindow() 
{
	ImGui::Begin("Layoff", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos(ImVec2(1280 - 512, 0));
	ImGui::SetWindowSize(ImVec2(512, 720));
	StatusDisplay();
	ImGui::Checkbox("Show screenConsole", &console->Display);
	if (ImGui::CollapsingHeader("Demos"))
	{
		if (ImGui::Button("swEyes", ImVec2(511, 0)))
		{
			if (!demoEyes)
				demoEyes = new SdlEyes();
		}
		if (ImGui::Button("Calc", ImVec2(511, 0)))
		{
			if (!demoCalc)
				demoCalc = new DemoCalc();
		}
		if (ImGui::Button("???", ImVec2(511, 0)))
		{
			if (!demoGame)
				demoGame = new DemoGame();
		}
	}
	ImGui::Spacing();
	if (ImGui::CollapsingHeader("System", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (IsWirelessEnabled)
		{
			if (ImGui::Button("Disable wireless", ImVec2(511, 0)))
			{
				SetSys::SetWirelessEnableFlag(false);
				IsWirelessEnabled = SetSys::GetWirelessEnableFlag();
			}
		}
		else if (ImGui::Button("Enable wireless", ImVec2(511, 0)))
		{
			SetSys::SetWirelessEnableFlag(true);
			IsWirelessEnabled = SetSys::GetWirelessEnableFlag();
		}
	}
	ImGui::End();
}

bool IdleLoop()
{
	HomeLongPressed = false;	
	HomePressed = false;
	while (OverlayAppletMainLoop())
	{		
		if (HomeLongPressed)
				return true;
		if(ntm->IsActive())
		{
			SDL_SetRenderDrawColor(sdl_render, 0, 0, 0, 0);
			SDL_RenderClear(sdl_render);
			ImGui::NewFrame();
			ntm->Render();
			ImGui::Render();
			ImGuiSDL::Render(ImGui::GetDrawData());
			SDL_RenderPresent(sdl_render);
		}

		//TEST
		if(!eventActive(notif))
			fatalSimple(MAKERESULT(255, 321));
		if(!R_FAILED(eventWait(notif, 5e+8)))
		{
			console->Print("Notif Event\n");
		}
	}
	return false;
}

bool WidgetDraw(UiItem** item)
{
	if (*item == nullptr)
		return false;
	
	bool result = (*item)->Draw();
	if (!result)
	{
		delete *item;
		*item = nullptr;
	}
	return result;
}
Service irec;
Result getIRec(Service *out, Service *rcv)
{
	IpcCommand c;
	ipcInitialize(&c); // Get IRec
	struct rq{
		u64 magic;
		u64 cmd_id;
	} *raw;

	raw = (rq*)serviceIpcPrepareHeader(rcv, &c, sizeof(*raw));
	raw->magic = SFCI_MAGIC;
	raw->cmd_id = 0;

	Result rc = serviceIpcDispatch(rcv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        struct rsp{
            u64 magic;
            u64 result;
        } *resp;

        serviceIpcParse(rcv, &r, sizeof(*resp));
        resp = (rsp*)r.Raw;

		serviceCreateSubservice(out, rcv, &r, 0);
		return resp->result;

	} else fatalSimple(rc);
}

Result regIRec(Service *irec)
{
	IpcCommand c;
	ipcInitialize(&c); // Reg IRec

	struct rq{
		u64 magic;
		u64 cmd_id;
	} *raw;

	raw = (rq*)serviceIpcPrepareHeader(irec, &c, sizeof(*raw));
	raw->magic = SFCI_MAGIC;
	raw->cmd_id = 0;
	Result rc = serviceIpcDispatch(irec);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        struct rsp{
            u64 magic;
            u64 result;
        } *resp;

        serviceIpcParse(irec, &r, sizeof(*resp));
        resp = (rsp*)r.Raw;
		return resp->result;
	} else fatalSimple(rc);
}

Result getIRecNotifEvent(Event *out, Service *irec)
{
	IpcCommand c;
	ipcInitialize(&c); // Get Handle
	struct rq2{
		u64 magic;
		u64 cmd_id;
		char unk[10];
	} *raw2;

	raw2 = (rq2*)serviceIpcPrepareHeader(irec, &c, sizeof(*raw2));
	raw2->magic = SFCI_MAGIC;
	raw2->cmd_id = 0;
	strcpy(raw2->unk, "overlay\0\0");
	Result rc = serviceIpcDispatch(irec);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        struct rsp{
            u64 magic;
            u64 result;
        } *resp;

        serviceIpcParse(irec, &r, sizeof(*resp));
        resp = (rsp*)r.Raw;
		eventLoadRemote(out, r.Handles[0], true);
		return resp->result;
	} else fatalSimple(rc);
}

void rectest()
{
	Service s;
	smGetService(&s, "ovln:rcv");
	getIRec(&irec, &s);
	regIRec(&irec);
	getIRecNotifEvent(notif, &irec);
}

bool LayoffMainLoop(ImGuiIO& io)
{
	//Get the wireless status only when opening the menu
	IsWirelessEnabled = SetSys::GetWirelessEnableFlag();
	statusTexTarget = new Texture();
	statusTexTarget->Source = SDL_CreateTexture(sdl_render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 512, 32);
	statusTexTarget->Surface = SDL_CreateRGBSurfaceWithFormat(0, 512, 32, 32, SDL_PIXELFORMAT_RGBA32);

	while (OverlayAppletMainLoop())
	{       
		if(!eventActive(notif))//TEST
			fatalSimple(MAKERESULT(255, 321));
		if(!R_FAILED(eventWait(notif, 1000000)))
		{
			console->Print("Notif Event\n");
		}
		// Battery % checks
		u64 ctimestamp = time(NULL);
		if(ltimestamp && ltimestamp - ctimestamp >= 5) { // When 5 secs passed since the last we checked, or it's the first time
			psmGetBatteryChargePercentage(&batteryPercentage);
			ltimestamp = ctimestamp;
		} else if (!ltimestamp){
			psmGetBatteryChargePercentage(&batteryPercentage);
			ltimestamp = ctimestamp;
		}
		

		SDL_SetRenderDrawColor(sdl_render, 0, 0, 0, 0);
		SDL_RenderClear(sdl_render);
		ImguiBindInputs(io);
		ImGui::NewFrame();
		/*if(!ntm->IDInUse("test")){
			Notification *n = new Notification("test", "Test notif", NULL, 10);
			ntm->Push(n);
			ntm->ShowLatest();
		}*/
		
		if (ActiveMode) //Draw the main window only if we have exclusive input like overlay would do
			LayoffMainWindow();
		
		if (console) 
			console->Draw();

		ntm->Render();
		
		bool DrewSomething = false; //Switch to active mode if the user closed all the widgets		
		DrewSomething |= WidgetDraw((UiItem**)&demoEyes);
		DrewSomething |= WidgetDraw((UiItem**)&demoCalc);
		DrewSomething |= WidgetDraw((UiItem**)&demoGame);

		ImGui::Render();
		ImGuiSDL::Render(ImGui::GetDrawData());
		SDL_RenderPresent(sdl_render);		
		svcSleepThread(33333333); //lock to ~30 fps
		
		if (HomeLongPressed || HomePressed)
			return true;
		
		if (hidKeysDown(CONTROLLER_P1_AUTO) & KEY_B)
			if (!DrewSomething)
				return true;
			else if (ActiveMode)
				SwitchToPassiveMode();
		
		if (!DrewSomething && !ActiveMode)
			SwitchToActiveMode();
	}
	return false; //The app should terminate
}

int main(int argc, char* argv[])
{    
	svcSleepThread(5e+9);
	__nx_win_init(); 

	romfsInit();    
	SdlInit();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	rectest();
	
	console = new ScreenConsole();
	ntm = new NotificationManager();

RESET:
	if(statusTexTarget)
		ImGuiSDL::FreeTexture(statusTexTarget);
	console->Print("Entering idle mode...\n");
	SDL_SetRenderDrawColor(sdl_render,0 ,0,0,0);
	SDL_RenderClear(sdl_render);
	SDL_RenderPresent(sdl_render);
	appletEndToWatchShortHomeButtonMessage(); //Unlock input for the foreground app	
	if (!IdleLoop())
		goto QUIT;
	HomeLongPressed = false;	
	HomePressed = false;
	SwitchToActiveMode(); //Lock input for the foreground app.
	
	console->Print("Entering active mode...\n");
	if (LayoffMainLoop(io))
		goto RESET;

QUIT: //does the overlay applet ever close ?
	if (demoEyes)
		delete demoEyes;
	
	delete console;
	
	SdlExit();
	return 0;
}

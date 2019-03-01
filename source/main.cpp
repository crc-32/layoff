#include <cstdlib>
#include <iostream>
#include <string>
#include "UI/UI.hpp"
#include <switch.h>

#define EVENT_FIRED(x) R_SUCCEEDED(eventWait(x,0))
#define EVENT_NOT_FIRED(x) R_FAILED(eventWait(x,0))

using namespace std;

extern "C" {
	extern u32 __start__;
	extern u32 __nx_applet_type;
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
		hidExit();
		smExit();
	}
}

#include "screenConsole.hpp"
ScreenConsole *console = nullptr;

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

bool HomeLongPressed = false;
bool HomePressed = false;
bool PowerPressed = false;
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

#include "demo/SdlEyes.hpp"
#include "demo/Calc.hpp"
SdlEyes *demoEyes = nullptr;
DemoCalc *demoCalc = nullptr;

void LayoffMainWindow() 
{
	ImGui::Begin("Layoff", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos(ImVec2(1280 - 512, 0));
	ImGui::SetWindowSize(ImVec2(512, 720));
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
			//TODO
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
		svcSleepThread(5e+8); //wait half a second
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

bool LayoffMainLoop(ImGuiIO& io)
{
	while (OverlayAppletMainLoop())
	{       
		SDL_SetRenderDrawColor(sdl_render, 0, 0, 0, 0);
		SDL_RenderClear(sdl_render);
		ImguiBindInputs(io);
		ImGui::NewFrame();
		
		if (ActiveMode) //Draw the main window only if we have exclusive input like overlay would do
			LayoffMainWindow();
		
		if (console) 
			console->Draw();
		
		bool DrewSomething = false; //Switch to active mode if the user closed all the widgets		
		DrewSomething |= WidgetDraw((UiItem**)&demoEyes);
		DrewSomething |= WidgetDraw((UiItem**)&demoCalc);
			
		ImGui::Render();
		ImGuiSDL::Render(ImGui::GetDrawData());
		SDL_RenderPresent(sdl_render);		
		
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
	void *heap;
	svcSetHeapSize(&heap, 0x10000000);
	svcSleepThread(5e+9);
	__nx_win_init(); 

	romfsInit();    
	SdlInit();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	
	console = new ScreenConsole();

RESET:
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

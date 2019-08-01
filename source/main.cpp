#include <cstdlib>
#include <iostream>
#include <string>
#include <iomanip>
#include "UI/UI.hpp"
#include <switch.h>
#include <time.h>
#include "NotificationManager.hpp"

#define EVENT_FIRED(x) R_SUCCEEDED(eventWait(x,0))
#define EVENT_NOT_FIRED(x) R_FAILED(eventWait(x,0))

using namespace std;

extern "C" {
    extern u32 __start__;

    u32 __nx_applet_type = AppletType_OverlayApplet;

    #define INNER_HEAP_SIZE 0x200000
    size_t nx_inner_heap_size = INNER_HEAP_SIZE;
    char   nx_inner_heap[INNER_HEAP_SIZE];

    void __libnx_initheap(void);
    void __appInit(void);
    void __appExit(void);

	extern void __nx_win_init(void);
	extern void __nx_win_exit(void);
}

void __appInit(void) {
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
}

void __appExit(void) {
    fsdevUnmountAll();
	fsExit();
	appletExit();
	hidsysExit();
	psmExit();
	hidExit();
	smExit();
}

void __libnx_initheap(void) {
	void*  addr = nx_inner_heap;
	size_t size = nx_inner_heap_size;

	/* Newlib */
	extern char* fake_heap_start;
	extern char* fake_heap_end;

	fake_heap_start = (char*)addr;
	fake_heap_end   = (char*)addr + size;
}

#include "screenConsole.hpp"
ScreenConsole *console = nullptr;
NotificationManager *ntm = nullptr;

Gfx *gfx;

bool ActiveMode;
//Get inputs while blocking the foreground app, the long press home button is not detected in this mode
void SwitchToActiveMode()
{	
	appletBeginToWatchShortHomeButtonMessage();
	ActiveMode = true;
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
}
u32 batteryPercentage = 0;
u64 ltimestamp;

//Events
bool HomeLongPressed = false;
bool HomePressed = false;
bool PowerPressed = false;

#include "PowerMenuWindow.hpp"
PowerMenuWindow *pwrwindow = nullptr;

//Settings
bool IsWirelessEnabled = false;
float BrightnessLevel = 0;
bool IsAutoBrightnessEnabled = false;
char CurIpAddress[16];

void UpdateIpAddress()
{
	u32 i = 0;
	nifmGetCurrentIpAddress(&i);	
	snprintf(CurIpAddress, 16 ,"%i.%i.%i.%i",
			i & 0xFF,(i >> 8) & 0xFF, 
			(i >> 16) & 0xFF,(i >> 24) & 0xFF);
}

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
/*Texture *statusTexTarget;
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
}*/
#include "demo/Calc.hpp"
#include "demo/CheatScreen.hpp"
DemoCalc *demoCalc = nullptr;
CheatScreen *cheatScreen = nullptr;

void LayoffMainWindow() 
{
	ImGui::Begin("Layoff", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos(ImVec2(1280 - 512, 0));
	ImGui::SetWindowSize(ImVec2(512, 720));
	//StatusDisplay();
	ImGui::Checkbox("Show screenConsole", &console->Display);
	if (ImGui::CollapsingHeader("Demos"))
	{
		if (ImGui::Button("Calc", ImVec2(511, 0)))
		{
			if (!demoCalc)
				demoCalc = new DemoCalc();
		}
		if (ImGui::Button("Cheat engine", ImVec2(511, 0)))
		{
			if (!cheatScreen)
				cheatScreen = new CheatScreen();
		}		
	}
	ImGui::Spacing();
	if (ImGui::CollapsingHeader("System", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (IsWirelessEnabled)
		{
			if (ImGui::Button("Disable wireless", ImVec2(511, 0)))
			{
				nifmSetWirelessCommunicationEnabled(false);
				nifmIsWirelessCommunicationEnabled(&IsWirelessEnabled);
			}
		}
		else if (ImGui::Button("Enable wireless", ImVec2(511, 0)))
		{
			nifmSetWirelessCommunicationEnabled(true);
			nifmIsWirelessCommunicationEnabled(&IsWirelessEnabled);
		}
		ImGui::Spacing();
		ImGui::Text("Ip address: %s",CurIpAddress);
		ImGui::Spacing();
		ImGui::Spacing();
		if (ImGui::Checkbox("Auto brightness", &IsAutoBrightnessEnabled))
		{
			if (IsAutoBrightnessEnabled)
			{
				if (R_FAILED(lblEnableAutoBrightnessControl()))
					console->Print("Auto brightness change failed\n");
			}
			else
			{
				if (R_FAILED(lblDisableAutoBrightnessControl()))
					console->Print("Auto brightness change failed\n");
			}
		}
		ImGui::Text("Brightness "); ImGui::SameLine();
		if (ImGui::SliderFloat("##Brightness", &BrightnessLevel, 0.0f, 1.0f, ""))
			if (R_FAILED(lblSetCurrentBrightnessSetting(BrightnessLevel)))
				console->Print("Brightness change failed\n");
	}
	ImGui::End();
}

void updateBattery()
{
	u64 ctimestamp = time(NULL);
	if(ltimestamp && ltimestamp - ctimestamp >= 5) { // When 5 secs passed since the last time we checked, or it's the first time
		psmGetBatteryChargePercentage(&batteryPercentage);
		ltimestamp = ctimestamp;
	} else if (!ltimestamp){
		psmGetBatteryChargePercentage(&batteryPercentage);
		ltimestamp = ctimestamp;
	}
	/*if(batteryPercentage > 15 && ntm->IDInUse("batlow"))
		ntm->HideID("batlow");
	else if (ntm->IDInUse("batlow"))
	{
		std::stringstream nText;
		nText << "Battery Low: " << to_string(batteryPercentage) << "%%";
		ntm->Push("batlow", nText.str(), "romfs:/notificationIcons/batLow.png", 0);
	}*/
}

bool IdleLoop()
{
	HomeLongPressed = false;	
	HomePressed = false;
	PowerPressed = false;
	while (OverlayAppletMainLoop())
	{		
		updateBattery();
		/*ntm->EventHandler(batteryPercentage);

		if(ntm->IsActive())
		{
			SDL_SetRenderDrawColor(sdl_render, 0, 0, 0, 0);
			SDL_RenderClear(sdl_render);
			ImGui::NewFrame();
			ntm->Render();
			ImGui::Render();
			ImGuiSDL::Render(ImGui::GetDrawData());
			SDL_RenderPresent(sdl_render);
		}*/

		if (PowerPressed || HomeLongPressed)
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
	//Get the system status only when opening the menu
	nifmIsWirelessCommunicationEnabled(&IsWirelessEnabled);
	UpdateIpAddress();	
	lblGetCurrentBrightnessSetting(&BrightnessLevel);
	lblIsAutoBrightnessControlEnabled(&IsAutoBrightnessEnabled);
	/*statusTexTarget = new Texture();
	statusTexTarget->Source = SDL_CreateTexture(sdl_render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 512, 32);
	statusTexTarget->Surface = SDL_CreateRGBSurfaceWithFormat(0, 512, 32, 32, SDL_PIXELFORMAT_RGBA32);
	*/
	while (OverlayAppletMainLoop())
	{       
		// Battery % checks
		updateBattery();
		//ntm->EventHandler(batteryPercentage);

		//SDL_SetRenderDrawColor(sdl_render, 0, 0, 0, 0);
		//SDL_RenderClear(sdl_render);
		ImguiBindInputs(io);
		gfx->StartRendering();
		
		if (pwrwindow)
		{
			bool ReturnAtTheEnd = false;
			if (!pwrwindow->Draw())
			{
				ReturnAtTheEnd = pwrwindow->WasIdle;
				delete pwrwindow;
				pwrwindow = nullptr;
			}
			ImGui::Render();
			//ImGuiSDL::Render(ImGui::GetDrawData());
			//SDL_RenderPresent(sdl_render);	
			gfx->EndRendering();	
			svcSleepThread(33333333); //lock to ~30 fps
			if (ReturnAtTheEnd)
				return true;
			continue;
		}
		
		if (ActiveMode) //Draw the main window only if we have exclusive input like overlay would do
			LayoffMainWindow();		
		
		if (console) 
			console->Draw();

		//ntm->Render();
		
		bool DrewSomething = false; //Switch to active mode if the user closed all the widgets		
		//DrewSomething |= WidgetDraw((UiItem**)&demoEyes);
		DrewSomething |= WidgetDraw((UiItem**)&demoCalc);
		//DrewSomething |= WidgetDraw((UiItem**)&demoGame);
		DrewSomething |= WidgetDraw((UiItem**)&cheatScreen);

		ImGui::Render();
		//SDL_RenderPresent(sdl_render);
		gfx->EndRendering();		
		svcSleepThread(33333333); //lock to ~30 fps
		
		if (HomeLongPressed || HomePressed)
			return true;
		
		if (hidKeysDown(CONTROLLER_P1_AUTO) & KEY_B){
			if (!DrewSomething)
				return true;
			else if (ActiveMode)
				SwitchToPassiveMode();
		}
		
		if (!DrewSomething && !ActiveMode)
			SwitchToActiveMode();
		
		if (PowerPressed)
			pwrwindow = new PowerMenuWindow(false);
	}
	return false; //The app should terminate
}
int main(int argc, char* argv[])
{    
	svcSleepThread(5e+9);
	__nx_win_init();

	romfsInit();
	gfx = new Gfx();
	nifmInitialize();
	lblInitialize();

	ovlnInitialize();
	
	console = new ScreenConsole();
	//ntm = new NotificationManager(console);

RESET:
	/*if(statusTexTarget)
		ImGuiSDL::FreeTexture(statusTexTarget);*/
	console->Print("Entering idle mode...\n");
	/*SDL_SetRenderDrawColor(sdl_render,0 ,0,0,0);
	SDL_RenderClear(sdl_render);
	SDL_RenderPresent(sdl_render);*/
	gfx->StartRendering();
	gfx->EndRendering();
	appletEndToWatchShortHomeButtonMessage(); //Unlock input for the foreground app	
	if (!IdleLoop())
		goto QUIT;
	HomeLongPressed = false;	
	HomePressed = false;
	if (PowerPressed && pwrwindow == nullptr)
		pwrwindow = new PowerMenuWindow(true);
	PowerPressed = false;
	SwitchToActiveMode(); //Lock input for the foreground app.
	
	console->Print("Entering active mode...\n");
	if (LayoffMainLoop(gfx->io))
		goto RESET;

QUIT: //does the overlay applet ever close ?
	/*if (demoEyes)
		delete demoEyes;*/
	
	delete console;
	if (pwrwindow)
		delete pwrwindow;
	
	lblExit();
	nifmExit();
	gfx->Exit();
	__nx_win_exit();
	ovlnExit();
	return 0;
}

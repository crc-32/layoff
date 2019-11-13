//Functions related to the general state of layoff
#pragma once

#include <CrossSwitch.h>
#include "UI/Window.hpp"

extern u64 kDown;
extern u64 kHeld;
static inline bool BackPressed() { return kDown & KEY_B; }
/* Credit: XorTroll (https://github.com/XorTroll/uLaunch/blob/master/Common/Include/os/os_HomeMenu.hpp) */
enum class GeneralChannelMessage : u32
{
	Invalid,
	HomeButton = 2, // Is this really HOME button? Never fires...
	Sleep = 3,
	Shutdown = 5,
	Reboot = 6,
	OverlayBrightValueChanged = 13,
	OverlayAutoBrightnessChanged = 14,
	OverlayAirplaneModeChanged = 15,
	HomeButtonHold = 16,
	OverlayHidden = 17,
};

struct SystemAppletMessage
{
	u32 magic; // "SAMS" -> System Applet message...?
	u32 unk;
	u32 message;
} PACKED;

namespace layoff::qlaunch {
	static constexpr u32 SAMSMagic = 0x534D4153;

	Result SendMessage(GeneralChannelMessage message);

	Result PowerShutdown();

	Result PowerSleep();

	Result PowerReboot();
}

namespace layoff {

	enum class OverlayMode
	{
		Active, //Layoff has the focus and the current app won't get inputs
		Idle, //Layoff is in idle mode and won't draw anything nor get inputs
		Passive, //A window has the focus but the current app will still receive inputs
	};
	
	bool IsHomeLongPressed();
	bool IsHomePressed();
	bool IsPowerPressed();
	
	OverlayMode GetCurrentMode();

	void SwitchToActiveMode();
	void SwitchToPassiveMode();
}

#if LAYOFF_LOGGING
	#include <string>
	void PrintLn(const std::string& s);
	void Print(const std::string& s);	
#else 
	#define PrintLn(x) {}
	#define Print(x) {}
#endif
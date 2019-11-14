#pragma once
#include <switch.h>

namespace layoff::ipc::qlaunch {

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

	static constexpr u32 SAMSMagic = 0x534D4153;

	Result SendMessage(GeneralChannelMessage message);
	Result PowerShutdown();
	Result PowerSleep();
	Result PowerReboot();

	Result SignalBrightnessChange();
	Result SignalAutoBrightnessChange();
	Result SignalAirplaneChange();
}
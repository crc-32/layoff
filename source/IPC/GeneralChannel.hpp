#pragma once
#include <switch.h>

namespace layoff::IPC::qlaunch {
	Result PowerShutdown();
	Result PowerSleep();
	Result PowerReboot();

	Result SignalBrightnessChange(float valuie);
	Result SignalAutoBrightnessChange(bool enabled);
	Result SignalAirplaneChange();

	Result SignalOverlayOpened();
	Result SignalOverlayClosed();
}
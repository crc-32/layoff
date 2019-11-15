#pragma once
#include <CrossSwitch.h>

namespace layoff::ipc::qlaunch {
	Result PowerShutdown();
	Result PowerSleep();
	Result PowerReboot();

	Result SignalBrightnessChange(float valuie);
	Result SignalAutoBrightnessChange(bool enabled);
	Result SignalAirplaneChange();
}
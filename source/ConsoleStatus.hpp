#pragma once

#include <CrossSwitch.h>

namespace layoff::console
{
	struct ConsoleStatus_t
	{		
		u32 BatteryLevel = 0;
		
		bool WirelessEnabled = false;		
		u32 IpAddress;
		char IpStr[16];
		constexpr bool Connected() { return IpAddress != 0; }
		
		float BrightnessLevel = 0;
		bool AutoBrightness = false;
		
		u64 Timestamp;
		TimeCalendarTime DateTime;
	};
	
	extern ConsoleStatus_t Status;
	
	void UpdateStatus();
	void RequestStatusUpdate();
}
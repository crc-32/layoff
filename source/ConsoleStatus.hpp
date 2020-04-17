#pragma once

#include <switch.h>

namespace layoff::console
{
	struct ConsoleStatus_t
	{		
		ChargerType chargerType;
		u32 BatteryLevel = 0;
		
		bool WirelessEnabled = false;		
		NifmInternetConnectionType connectionType;
		NifmInternetConnectionStatus connectionStatus;
		u32 ConnectionStrenght;
		u32 IpAddress;

		char IpStr[16];
		constexpr bool Connected()
		{
			return IpAddress!= 0 && connectionStatus == NifmInternetConnectionStatus_Connected &&
				(WirelessEnabled || connectionType == NifmInternetConnectionType_Ethernet); 
		}
		
		float BrightnessLevel = 0;
		bool AutoBrightness = false;
		
		u64 Timestamp;
		TimeCalendarTime DateTime;
	};
	
	extern ConsoleStatus_t Status;
	
	void UpdateStatus();
	void RequestStatusUpdate();
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ConsoleStatus.hpp"

namespace layoff::console
{
	layoff::console::ConsoleStatus_t Status;
	static u64 LastUpdate = 0;		
		
	void RequestStatusUpdate()
	{
		LastUpdate = 0;
	}
	
	static inline void IpAddrToString(u32 i, char* buf)
	{
		snprintf(buf, 16 ,"%i.%i.%i.%i",
			i & 0xFF,			(i >> 8) & 0xFF, 
			(i >> 16) & 0xFF,	(i >> 24) & 0xFF);
	}
	
	static inline void DoStatusUpdate()
	{
#if __SWITCH__
		psmGetBatteryChargePercentage(&Status.BatteryLevel);
		
		nifmIsWirelessCommunicationEnabled(&Status.WirelessEnabled);
		nifmGetCurrentIpAddress(&Status.IpAddress);	
		IpAddrToString(Status.IpAddress, Status.IpStr);
				
		lblGetCurrentBrightnessSetting(&Status.BrightnessLevel);
		lblIsAutoBrightnessControlEnabled(&Status.AutoBrightness);	

		timeGetCurrentTime(TimeType_UserSystemClock, &Status.Timestamp);
		timeToCalendarTimeWithMyRule(Status.Timestamp, &Status.DateTime, nullptr);
#else
#endif
	}
	
	void UpdateStatus()
	{
		if (time(NULL) - LastUpdate <= 5) return;
		LastUpdate = time(NULL);		
		
		DoStatusUpdate();
	}	
}

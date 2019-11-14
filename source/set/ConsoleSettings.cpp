#include "ConsoleSettings.hpp"
#include <switch.h>
#include "../IPC/GeneralChannel.hpp"
#include "../ConsoleStatus.hpp"
#include "../utils.hpp"

using namespace layoff::ipc;

namespace layoff::set
{
	void SetBrightness(float value)
	{
		if (R_FAILED(lblSetCurrentBrightnessSetting(value)))
			Print("Brightness change failed\n");
		qlaunch::SignalBrightnessChange();
		layoff::console::RequestStatusUpdate();
	}

	void SetAutoBrightness(bool value)
	{
		Result rc = 0;

		if (value)
			rc = lblEnableAutoBrightnessControl();
		else
			rc = lblDisableAutoBrightnessControl();

		if (R_FAILED(rc))
			Print("Auto brightness change failed\n");
		qlaunch::SignalAutoBrightnessChange();
		layoff::console::RequestStatusUpdate();
	}

	void SetWireless(bool enabled)
	{		
		if (R_FAILED(nifmSetWirelessCommunicationEnabled(enabled)))
			Print("Set wireless comms enabled failed\n");
		//TODO: Find a proper way of setting airplane mode, this will only disable wireless
		//qlaunch::SignalAirplaneChange();
		layoff::console::RequestStatusUpdate();
	}
}
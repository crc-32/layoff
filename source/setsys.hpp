#pragma once
#include <switch.h>

namespace SetSys
{
	bool GetBluetoothEnableFlag();
	bool GetNFCEnableFlag();
	bool GetWirelessEnableFlag();
	
	void SetBluetoothEnableFlag(bool);
	void SetNFCEnableFlag(bool);
	void SetWirelessEnableFlag(bool);
}
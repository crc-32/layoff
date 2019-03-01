#include "setsys.hpp"

bool SetSys::GetBluetoothEnableFlag()
{
	setsysInitialize();
	bool result;
	if (R_FAILED(setsysGetFlag(SetSysFlag_BluetoothEnable, &result)))
		fatalSimple(MAKERESULT(255, 66));
	setsysExit();
	return result;
}

bool SetSys::GetNFCEnableFlag()
{
	setsysInitialize();
	bool result;
	if (R_FAILED(setsysGetFlag(SetSysFlag_NfcEnable, &result)))
		fatalSimple(MAKERESULT(255, 66));
	setsysExit();
	return result;
}

bool SetSys::GetWirelessEnableFlag()
{
	setsysInitialize();
	bool result;
	if (R_FAILED(setsysGetFlag(SetSysFlag_WirelessLanEnable, &result)))
		fatalSimple(MAKERESULT(255, 66));
	setsysExit();
	return result;
}

void SetSys::SetBluetoothEnableFlag(bool value)
{
	setsysInitialize();
	if (R_FAILED(setsysSetFlag(SetSysFlag_BluetoothEnable, value)))
		fatalSimple(MAKERESULT(255, 66));
	setsysExit();
}

void SetSys::SetNFCEnableFlag(bool value)
{
	setsysInitialize();
	if (R_FAILED(setsysSetFlag(SetSysFlag_NfcEnable, value)))
		fatalSimple(MAKERESULT(255, 66));
	setsysExit();
}

void SetSys::SetWirelessEnableFlag(bool value)
{
	setsysInitialize();
	if (R_FAILED(setsysSetFlag(SetSysFlag_WirelessLanEnable, value)))
		fatalSimple(MAKERESULT(255, 66));
	setsysExit();
}

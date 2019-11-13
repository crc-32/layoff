#include "utils.hpp"
namespace layoff::qlaunch {
	/* Credit: XorTroll */

	Result SendMessage(GeneralChannelMessage message)
	{
		Result rc = 0;
#if __SWITCH__
		AppletStorage st;
		rc = appletCreateStorage(&st, 1024);
		if(R_SUCCEEDED(rc))
		{
			SystemAppletMessage sams = {};
			sams.magic = SAMSMagic;
            sams.unk = 1;
			sams.message = (u32) message;
			
			rc = appletStorageWrite(&st, 0, &sams, sizeof(sams));
            u8 buf[1012] = {0};
            *(u32*)buf = 1;
            rc = appletStorageWrite(&st, sizeof(sams), buf, sizeof(buf));

			if(R_SUCCEEDED(rc)) appletPushToGeneralChannel(&st);
			appletStorageClose(&st);
		}
#endif
		return rc;
	}

	Result PowerShutdown()
	{
		return SendMessage(GeneralChannelMessage::Shutdown);
	}

	Result PowerSleep()
	{
		return SendMessage(GeneralChannelMessage::Sleep);
	}

	Result PowerReboot()
	{
		return SendMessage(GeneralChannelMessage::Reboot);
	}
}
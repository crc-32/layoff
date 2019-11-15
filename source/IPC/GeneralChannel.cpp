#include "GeneralChannel.hpp"
#include <new>

namespace layoff::ipc::qlaunch {

	/* Credit: XorTroll (https://github.com/XorTroll/uLaunch/blob/master/Common/Include/os/os_HomeMenu.hpp) */
	static constexpr u32 SAMSMagic = 0x534D4153;

	struct SystemAppletMessage
	{
		u32 magic; // "SAMS" -> System Applet message...?
		u32 unk;
		u32 message;
	} PACKED;
	static_assert(sizeof(SystemAppletMessage) == 12);

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

	template <typename ...T>
	static inline Result SendMessage(GeneralChannelMessage message, T ...args)
	{
#if __SWITCH__
		static_assert(((sizeof(args) == sizeof(u32)) && ...), "Each argument of SendMessage must have the same size as u32");
		static_assert(sizeof...(args) < 1012 / 4, "The payload buffer is 1012 bytes long");

		AppletStorage st;
		auto rc = appletCreateStorage(&st, 1024);
		if (R_SUCCEEDED(rc))
		{
			SystemAppletMessage sams = {};
			sams.magic = SAMSMagic;
			sams.unk = 1;
			sams.message = (u32)message;

			rc = appletStorageWrite(&st, 0, &sams, sizeof(sams));
			//Total official msg size is 1024 bytes, so the actual payload is 1012 bytes
			//the first u32 has to be set to 1
			u32 buf[1012 / 4] = { 1, *std::launder(reinterpret_cast<u32*>(&args))... };
			rc = appletStorageWrite(&st, sizeof(sams), buf, sizeof(buf));

			if (R_SUCCEEDED(rc)) appletPushToGeneralChannel(&st);
			appletStorageClose(&st);
		}
		return rc;
#endif
		return 0;
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

	Result SignalBrightnessChange(float value)
	{		
		return SendMessage(GeneralChannelMessage::OverlayBrightValueChanged, value);
	}

	Result SignalAutoBrightnessChange(bool enabled)
	{
		return SendMessage(GeneralChannelMessage::OverlayAutoBrightnessChanged, (u32)enabled);
	}

	Result SignalAirplaneChange()
	{
		return SendMessage(GeneralChannelMessage::OverlayAirplaneModeChanged);
	}
}
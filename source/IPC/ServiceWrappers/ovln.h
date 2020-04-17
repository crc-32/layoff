/**
 * @file ovln.h
 * @brief Overlay notifications (ovln) service IPC wrapper.
 * @author crc-32
 */
#pragma once
#include <switch.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef enum {
		OvlnNotificationType_ThemeChanged = 0x0009, //Payload[0] = E0, Payload[1] = 0x68 for dark theme, A0 and 0x50 for white. More data follows (equal for both)
		OvlnNotificationType_Battery = 0x0bb9,	//Received when power is (un)plugged, Payload[0] is battery %
		OvlnNotificationType_LowBat = 0x0bba,
		OvlnNotificationType_Volume = 0x03e8,  //Payload[0] is volume value 0 to F, Payload[4] is 1 if + button, 2 if - 
		OvlnNotificationType_Screenshot = 0x1f44,
		OvlnNotificationType_ScreenshotFail = 0x1f43,
		OvlnNotificationType_ScreenshotUnk = 0x1f41,
		OvlnNotificationType_Video = 0x1fa4,
		OvlnNotificationType_VideoFail = 0x1fa6
	} OvlnNotificationType;

	typedef struct {
		u32 Type;
		u32 Unknown; //Seems to always be 8
		u8 Payload[0x78]; //Not sure if this is all payload, towards the end there seems to be more data
		u64 Tick; 
	} PACKED OvlnNotificationWithTick;

	_Static_assert(sizeof(OvlnNotificationWithTick) == 0x88, "");

	Result ovlnInitialize();
	void ovlnExit();

	Result ovlnIReceiverReceiveWithTick(OvlnNotificationWithTick* out);
	Result ovlnIReceiverGetReceiveEventHandle(Event* out);
#ifdef __cplusplus
}
#endif
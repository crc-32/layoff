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
		BatteryNotifType = 0xb90b,
		LowBatNotifType = 0xba0b,
		VolumeNotifType = 0xe803,
		ScreenshotNotifType = 0x441f,
		ScreenshotFailNotifType = 0x431f,
		ScreenshotUnkNotifType = 0x411f,
		VideoNotifType = 0xa41f,
		VideoFailNotifType = 0xa61f
	} IReceiverNotifType;

	typedef struct {
		u8 data[0x88];
	} OvlnNotificationWithTick;

	Result ovlnInitialize();
	void ovlnExit();

	Result ovlnIReceiverReceiveWithTick(OvlnNotificationWithTick* out);
	Result ovlnIReceiverGetReceiveEventHandle(Event* out);
#ifdef __cplusplus
}
#endif
/**
 * @file ovln.h
 * @brief Overlay notifications (ovln) service IPC wrapper.
 * @author crc-32
 */
#pragma once
#include <switch.h>

typedef enum {
	BatteryNotifType = 0xb90b,
	LowBatNotifType = 0xba0b,
	VolumeNotifType  = 0xe803,
    ScreenshotNotifType = 0x441f,
	ScreenshotFailNotifType = 0x431f,
    ScreenshotUnkNotifType = 0x411f,
    VideoNotifType = 0xa41f,
	VideoFailNotifType = 0xa61f
} IReceiverNotifType;

typedef struct {
	u16 type;
	u16 content;
} IReceiverNotification;

Result ovlnInitialize();
void ovlnExit();

Result ovlnIReceiverGetNotification(IReceiverNotification *out);
Result ovlnIReceiverGetEvent(Event *out);
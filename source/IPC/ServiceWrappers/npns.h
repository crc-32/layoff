/**
 * @file npns.h
 * @brief Nintendo push notification (npns) service IPC wrapper.
 * @author crc-32
 */
#pragma once
#include <switch.h>

#ifdef __cplusplus
extern "C" {
#endif

	Result npnsInitialize();
	void npnsExit();

#ifdef __cplusplus
}
#endif
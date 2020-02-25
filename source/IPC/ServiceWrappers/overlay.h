#pragma once
#include <layoff.h>
#include "../../../service/source/OverlayServiceTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

	Result overlayinitialize();
	void overlayExit();

	Result overlayGetEvent(Event* evt);

	Result overlayLock();
	Result overlayUnlock();

	Result overlayGetQueueStatus(IPCQUeueStatus* st);
	Result overlayPopPrintQueue(char* str, u32 len, u32* outLen);
	Result overlayPopUIQueue(IPCUIPush* ui, u8* data, u32 len);
	Result overlayReadClientQueue(IPCClient* clients, u32 len, u32* read);
	Result overlayReadNotifsQueue(SimpleNotification* notifs, u32 len, u32* read);

	Result overlayPushUiStateChange(const IPCUIEvent* evt);

#ifdef __cplusplus
}
#endif
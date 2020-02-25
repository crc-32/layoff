#include "overlay.h"

static Service srv;

Result overlayinitialize()
{
	return smGetService(&srv, "overlay");;
}

void overlayExit()
{
	serviceClose(&srv);
}

Result overlayGetEvent(Event* evt)
{
	Handle event = INVALID_HANDLE;
	Result rc = serviceDispatch(&srv, LayoffOverlayCmdID_AcquireNewDataEvent,
		.out_handle_attrs = { SfOutHandleAttr_HipcCopy },
		.out_handles = &event
	);

	if (R_SUCCEEDED(rc))
		eventLoadRemote(evt, event, true);

	return rc;
}

Result overlayLock()
{
	return serviceDispatch(&srv, LayoffOverlayCmdID_Lock);
}

Result overlayUnlock()
{
	return serviceDispatch(&srv, LayoffOverlayCmdID_Unlock);
}

Result overlayGetQueueStatus(IPCQUeueStatus* st)
{
	return serviceDispatchOut(&srv, LayoffOverlayCmdID_GetQueueStatus, *st);
}

Result overlayPopPrintQueue(char* str, u32 len, u32* outLen)
{
	return serviceDispatchOut(&srv, LayoffOverlayCmdID_PopPrintQueue, *outLen,
		.buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
		.buffers = { { str, len } }
	);
}

Result overlayPopUIQueue(IPCUIPush* ui, u8* data, u32 len)
{
	return serviceDispatchOut(&srv, LayoffOverlayCmdID_PopUIQueue, *ui,
		.buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
		.buffers = { { data, len} }
	);
}

Result overlayReadClientQueue(IPCClient* clients, u32 len, u32* read)
{
	return serviceDispatchOut(&srv, LayoffOverlayCmdID_ReadClientQueue, *read,
		.buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
		.buffers = { { clients, len} }
	);
}

Result overlayReadNotifsQueue(SimpleNotification* notifs, u32 len, u32* read)
{
	return serviceDispatchOut(&srv, LayoffOverlayCmdID_ReadNotifQueue, *read,
		.buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
		.buffers = { { notifs, len} }
	);
}

Result overlayPushUiStateChange(const IPCUIEvent* evt)
{
	IPCUIEvent e = *evt;
	return serviceDispatchIn(&srv, LayoffOverlayCmdID_PushUIStateChange, e);
}
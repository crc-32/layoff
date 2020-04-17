#pragma once
#include <switch.h>
#include <layoff.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef enum {
		LayoffOverlayCmdID_Lock = 1,
		LayoffOverlayCmdID_Unlock,
		LayoffOverlayCmdID_GetQueueStatus,
		LayoffOverlayCmdID_PopPrintQueue,
		LayoffOverlayCmdID_PopUIQueue,
		LayoffOverlayCmdID_ReadClientQueue,
		LayoffOverlayCmdID_ReadNotifQueue,
		LayoffOverlayCmdID_AcquireNewDataEvent,

		LayoffOverlayCmdID_PushUIStateChange,
	} LayoffOverlayCmdID;

	typedef struct
	{
		LayoffUIEvent evt;
		LayoffIdentifier Client;
	} PACKED IPCUIEvent;

	typedef enum
	{
		OverlayAction_Connected,
		OverlayAction_Disconnected,
	} OverlayAction;

	typedef struct
	{
		OverlayAction action;
		LayoffIdentifier id;
		LayoffName name;
	} PACKED IPCClient;

	typedef struct 
	{
		u16 Prints;
		u16 Notifs;
		u16 Clients;
		u16 UI;
	} PACKED IPCQUeueStatus;

	typedef struct 
	{
		LayoffIdentifier client;
		LayoffUIHeader header;
		u32 BufferLen;
	} PACKED IPCUIPush;

#ifdef __cplusplus
}
#endif
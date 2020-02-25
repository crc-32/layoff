#include <sstream>
#include <atomic>

#include "../NotificationManager.hpp"
#include "../utils.hpp"
#include "Clients.hpp"
#include "ServiceWrappers/overlay.h"

namespace 
{
	Thread ipcThread;
	std::atomic<bool> threadRunning;

#define ASSERTRESULT(rc, desc) do { \
	if (R_FAILED(rc)) { \
		PrintLn( desc " failed with " + R_STRING(rc));\
		return;	\
	}} while (0)

	static void ipcMain(void*)
	{
		Result rc;
		rc = overlayinitialize();
		ASSERTRESULT(rc, "initialize");

		Event evt;
		rc = overlayGetEvent(&evt);
		ASSERTRESULT(rc, "get event");

		while (threadRunning)
		{
			rc = eventWait(&evt, 1e+9); //every second check if the thread should exit
			if (R_VALUE(rc) == KERNELRESULT(TimedOut)) continue;
			ASSERTRESULT(rc, "eventWait");

			PrintLn("Event fired");

			overlayLock();

			IPCQUeueStatus status = {};
			rc = overlayGetQueueStatus(&status);
			ASSERTRESULT(rc, "getQueueStatus");

			for (u32 i = 0; i < status.Prints; i++)
			{
				char text[200]; u32 written = 0;
				rc = overlayPopPrintQueue(text, sizeof(text), &written);
				ASSERTRESULT(rc, "overlayPopPrintQueue");
				PrintLn(text);
			}
			if (status.Notifs) {
				SimpleNotification* notifs = new SimpleNotification[status.Notifs];
				u32 read = 0;
				rc = overlayReadNotifsQueue(notifs, sizeof(SimpleNotification) * status.Notifs, &read);
				ASSERTRESULT(rc, "ReadNotifsQueue");
				for (u32 i = 0; i < read; i++)
					layoff::notif::PushSimple(notifs[i].message, notifs[i].name.str);
				delete[] notifs;
			}
			if (status.Clients)
			{
				IPCClient* clients = new IPCClient[status.Clients];
				u32 read = 0;
				rc = overlayReadClientQueue(clients, sizeof(IPCClient) * status.Clients, &read);
				ASSERTRESULT(rc, "overlayReadClientQueue");
				for (u32 i = 0; i < read; i++)
					layoff::IPC::ClientAction(clients[i]);
				delete[] clients;
			}
			for (u32 i = 0; i < status.UI; i++)
			{
				u8 buf[200]; IPCUIPush push = {};
				rc = overlayPopUIQueue(&push, buf, sizeof(buf));
				ASSERTRESULT(rc, "overlayPopUIQueue");
				if (sizeof(buf) < push.BufferLen)
					PrintLn("IPCUIPush buffer too small !");
				else
					layoff::IPC::AddUIPanel(push, buf);
			}		
			
			overlayUnlock();
		}

		overlayExit();
	}
}

namespace layoff::IPC {

    void LaunchThread() 
    {
		InitClients();
		threadRunning = true;
        //From switchbrew: priority is a number 0-0x3F. Lower value means higher priority.
        //Main thread is 0x2C
        threadCreate(&ipcThread, &ipcMain, NULL, NULL, 0x4000, 0x2D, -2);
        threadStart(&ipcThread);
    }

    void RequestAndWaitExit() 
    {
		threadRunning = false;
        threadWaitForExit(&ipcThread);
    }
}
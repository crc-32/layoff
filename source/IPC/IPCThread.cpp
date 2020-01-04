#include <nxExt.h>
#include <sstream>
#include "../utils.hpp"
#include "IPCThread.hpp"
#include "servers/Notification.hpp"

Thread notif_thread;
IpcServer notif_server;

static void IPCMain(void* _arg) 
{
	Result rc;
    IpcServer* ipcSrv = (IpcServer*)_arg;
    while(true)
    {
        rc = ipcServerProcess(ipcSrv, &IPC::services::NotificationService::HandleRequest, _arg);
        if(R_FAILED(rc))
        {
            if(rc == KERNELRESULT(Cancelled))
            {
                return;
            }
            if(rc != KERNELRESULT(ConnectionClosed))
            {
                #ifdef LAYOFF_LOGGING
				std::stringstream msg;
				msg << "Unhandled Error in IPC thread: " << R_MODULE(rc) << "-" << R_DESCRIPTION(rc);
				PrintLn(msg.str());
				#endif
            }
        }
    }
}

void IPC::LaunchThread() 
{
	//From switchbrew: priority is a number 0-0x3F. Lower value means higher priority.
	//Main thread is 0x2C
	ipcServerInit(&notif_server, "layoff:n", 32);
	threadCreate(&notif_thread, &IPCMain, &notif_server, NULL, 0x4000, 0x2D, -2);
	threadStart(&notif_thread);
}

void IPC::RequestAndWaitExit() 
{
	threadWaitForExit(&notif_thread);
}
#include <stratosphere.hpp>
#include "IPCThread.hpp"
#include "servers\Notification.hpp"

static auto &&IPCManager = WaitableManager<DefaultManagerOptions>(1);
static HosThread IPCThread;

static void IPCMain(void* _arg) 
{
	IPCManager.AddWaitable(new ServiceServer<IPC::services::NotificationService>("layoff:n", 8));
	IPCManager.Process();
}

void IPC::LaunchThread() 
{
	//From switchbrew: priority is a number 0-0x3F. Lower value means higher priority.
	//Main thread is 0x2C
	IPCThread.Initialize(IPCMain, nullptr, 0x4000, 0x2D); 
	IPCThread.Start();
}

void IPC::RequestAndWaitExit() 
{
	IPCManager.RequestStop();
	IPCThread.Join();
}
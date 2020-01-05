#include "Notification.hpp"
#include "../../utils.hpp"
#include <layoff.h>
#include <string>
#include <sstream>
#include <vector>

namespace IPC {
	namespace services {
	
		Result NotificationService::NotifySimple(const IpcServerRequest* r)
		{
			if (r->data.size >= sizeof(SimpleNotification))
			{
				SimpleNotification *notif = (SimpleNotification*)r->data.ptr;
				PrintLn(notif->message);
				return 0;
			}
			std::stringstream a;
			a << r->data.size;
			PrintLn(a.str());
			//TODO
			//NotificationsManager::PushSimple(string);
			//Be sure to use a mutex !

			return MAKERESULT(255,3);
		}

		Result NotificationService::NotifyEx(const IpcServerRequest* r)
		{
			//TODO
			
			/*
			Notification* n = (Notification*)MessageData.buffer;
			
			std::string title((const char*)n->Data, n->TitleLen);
			std::string msg((const char*)n->Data + n->TitleLen, n->MsgLen);
			
			std::vector<u8> image(n->ImgLen);
			std::memcpy(image.data(), n->Data + n->TitleLen + n->MsgLen, n->ImgLen);
			*/

			//NotificationsManager::Push(title, msg, image);

			return 0;
		}

		Result NotificationService::HandleRequest(void* arg, const IpcServerRequest* r, u8* out_data, size_t* out_dataSize)
		{
			IpcServer* ipcSrv = (IpcServer*)arg;
			switch(r->data.cmdId)
			{
				case LayoffCmdId_NotifyEx:
					return MAKERESULT(255, 2);
				case LayoffCmdId_NotifySimple:
					return NotifySimple(r);
				default:
					return MAKERESULT(255,1);
			}
		}
	
	}
}
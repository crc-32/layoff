#include "Notification.hpp"
#include <string>
#include <string.h>
#include <vector>

namespace IPC {
	namespace services {
	
		Result NotificationService::NotifySimple(const IpcServerRequest* r)
		{
			char cstr[r->data.size + 1] = {0};
			strncpy(cstr, (char*)r->data.ptr, r->data.size);
			std::string string(cstr);
			
			//TODO
			//NotificationsManager::PushSimple(string);
			//Be sure to use a mutex !

			return 0;
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
				case CommandId::NotifyExId:
					return MAKERESULT(255, 2);
				case CommandId::NotifySimpleId:
					return NotifySimple(r);
				default:
					return MAKERESULT(255,1);
			}
		}
	
	}
}
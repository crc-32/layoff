#include "Notification.hpp"
#include <string>
#include <vector>

namespace IPC {
	namespace services {
	
		Result NotificationService::NotifySimple(InBuffer<s8> Message)
		{
			auto cstr = (const char*)Message.buffer;
			std::string string(cstr);
			
			//TODO
			//NotificationsManager::PushSimple(string);
			//Be sure to use a mutex !

			return string.length();
		}

		Result NotificationService::NotifyEx(InBuffer<u8> MessageData)
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
	
	}
}
#pragma once

#include <string>
#include <vector>
#include <switch.h>

#include "IPC/IPCLock.hpp"

namespace layoff::notif {
  
	void Initialize();

	struct Notification 
	{
	public:
		std::string message = "";
		std::string author = "";
		std::vector<u8> image;
		s64 ts;

		bool HasImage() const { return image.size() != 0; }
	};
	
	void PushSimple(const std::string& content, const std::string& author);
	void Push(Notification&& notif);
	s64 LastNotifTs();

	using NotifLock = layoff::IPC::ObjLock<std::vector<Notification>>;

	NotifLock LockNotifs();
	void ClearHistory();
}
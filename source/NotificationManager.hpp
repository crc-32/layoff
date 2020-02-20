#pragma once

#include <string>
#include <map>
#include <vector>
#include <switch.h>

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

	struct NotifLock
	{
		std::vector<Notification>& list;

		NotifLock(std::vector<Notification>& l); //Locks the notification mutex
		~NotifLock(); //Unlocks the mutex

		NotifLock(NotifLock& other) = delete;
		NotifLock& operator=(NotifLock other) = delete;
	};

	NotifLock LockNotifs();
	void ClearHistory();
}
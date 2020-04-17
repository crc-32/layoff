#include "NotificationManager.hpp"
#include <sstream>
#include <algorithm>
#include <atomic>
#include "utils.hpp"
#include <time.h>

namespace layoff::notif {
	static std::atomic<bool> newNotifs;
	static std::vector<Notification> notifications;
	static Mutex notifMutex;

	void PushSimple(const std::string& content, const std::string& author)
	{
		Push({ content, author, {}, time(NULL) });
	}

	void Push(Notification&& notif)
	{
		auto ts = notif.ts;
		PrintLn(notif.message + " " + notif.author + " " + std::to_string(ts));
		auto&& lock = LockNotifs();
		lock.obj.push_back(std::move(notif));
		newNotifs = true;
	}

	bool HasNewNotifs()
	{
		auto&& lock = LockNotifs();

		bool res = newNotifs;
		if (res) 
			newNotifs = false;
		return res;
	}

	NotifLock LockNotifs()
	{
		return NotifLock(notifications, notifMutex);
	}

	void Initialize()
	{
		mutexInit(&notifMutex);
	}
}
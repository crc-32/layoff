#include "NotificationManager.hpp"
#include <sstream>
#include <algorithm>
#include <atomic>
#include "utils.hpp"
#include <time.h>

namespace layoff::notif {
	static std::atomic<s64> lastTs = 0;
	static std::vector<Notification> notifications;
	static Mutex notifMutex;

	void PushSimple(const std::string& content, const std::string& author)
	{
		Push({ content, author, {}, time(NULL) });
	}

	void Push(Notification&& notif)
	{
		auto ts = notif.ts;
		PrintLn(notif.message + " " + notif.author + " " + std::to_string(notif.ts));
		LockNotifs().obj.push_back(std::move(notif));
		lastTs = ts;
	}

	s64 LastNotifTs()
	{
		return lastTs;
	}

	NotifLock LockNotifs()
	{
		return NotifLock(notifications, notifMutex);
	}
	
	void ClearHistory()
	{
		LockNotifs().obj.clear();
	}

	void Initialize()
	{
		mutexInit(&notifMutex);
	}
}
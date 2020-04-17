#include "overlay_service.hpp"
#include "../../source/IPC/ErrorCodes.h"

#include "Clients.hpp"

namespace services
{
	Event OverlayService::newData;
	Mutex OverlayService::mutex;

	std::queue<std::string> OverlayService::printQueue;
	std::queue<IPCClient> OverlayService::clientQueue;
	std::queue<services::OverlayService::ClientUIPush> OverlayService::UIQueue;
	std::queue<SimpleNotification> OverlayService::notifQueue;

	void OverlayService::InitializeStatics() 
	{
		ams::Result rc = eventCreate(&newData, true);
		if (rc.IsFailure())
			fatalThrow(rc.GetValue());
		mutexInit(&mutex);
	}

	void OverlayService::FinalizeStatics() 
	{
		eventClose(&newData);
	}

	ams::Result OverlayService::LockEvents()
	{
		mutexLock(&mutex);
		return ams::ResultSuccess();
	}

	ams::Result OverlayService::UnlockEvents()
	{
		mutexUnlock(&mutex);
		return ams::ResultSuccess();
	}

	ams::Result OverlayService::GetQueueStatus(sf::Out<IPCQUeueStatus> status)
	{
		IPCQUeueStatus res;
		res.Prints = printQueue.size();
		res.Notifs = notifQueue.size();
		res.Clients = clientQueue.size();
		res.UI = UIQueue.size();
		*status = res;
		return ams::ResultSuccess();
	}

	ams::Result OverlayService::PopPrintQueue(sf::OutBuffer buf, sf::Out<u32> WrittenLen)
	{
		auto& t = printQueue.front();
		u32 toCopy = std::max(buf.GetSize(), t.size() + 1);

		std::memcpy(buf.GetPointer(), t.c_str(), toCopy);
		buf.GetPointer()[toCopy - 1] = '\0';
		*WrittenLen = toCopy;

		printQueue.pop();
		return ams::ResultSuccess();
	}

	ams::Result OverlayService::PopUIQueue(sf::OutBuffer buf, sf::Out<IPCUIPush> out)
	{
		auto& t = UIQueue.front();
		if (buf.GetSize() < t.data.size())
			return ERR_BUFFER_TOO_SMALL;

		IPCUIPush res;
		res.client = t.client;
		res.BufferLen = t.data.size();
		res.header = t.header;
		std::memcpy(buf.GetPointer(), t.data.data(), t.data.size());
		*out = res;

		UIQueue.pop();
		return ams::ResultSuccess();
	}

	template<typename T>
	static inline u32 CopyToIPCBuf(std::queue<T>& queue, sf::OutBuffer& buf)
	{
		static_assert(std::is_pod<T>::value);

		int count = 0;
		u8* out = buf.GetPointer();
		u8* limit = out + buf.GetSize();
		while (queue.size())
		{
			T& obj = queue.front();

			if (out + sizeof(obj) > limit)
				break;
			std::memcpy(out, &obj, sizeof(obj));

			out += sizeof(obj);
			queue.pop();
			count++;
		}
		return count;
	}

	ams::Result OverlayService::ReadClientQueue(sf::OutBuffer buf, sf::Out<u32> WrittenCount)
	{		
		*WrittenCount = CopyToIPCBuf(clientQueue, buf);
		return ams::ResultSuccess();
	}

	ams::Result OverlayService::ReadNotifQueue(sf::OutBuffer buf, sf::Out<u32> WrittenCount) 
	{
		*WrittenCount = CopyToIPCBuf(notifQueue, buf);
		return ams::ResultSuccess();
	}

	ams::Result OverlayService::AcquireNewDataEvent(sf::OutCopyHandle evt) {
		*evt = newData.revent;
		return ams::ResultSuccess();
	}

	ams::Result OverlayService::PushUIStateChange(IPCUIEvent evt)
	{
		return layoff::IPC::clients::PushUIEventData(evt.Client, evt.evt);
	}

	void OverlayService::PrintLn(const std::string&& str)
	{
		layoff::IPC::ScopeLock lock(mutex);
		printQueue.push(std::move(str));
		eventFire(&newData);
	}

	void OverlayService::ClientAction(const IPCClient&& cli)
	{
		layoff::IPC::ScopeLock lock(mutex);
		clientQueue.push(std::move(cli));
		eventFire(&newData);
	}
	
	void OverlayService::UIPush(const ClientUIPush&& p)
	{
		layoff::IPC::ScopeLock lock(mutex);
		UIQueue.push(std::move(p));
		eventFire(&newData);
	}

	void OverlayService::NotifSimple(const SimpleNotification& n) 
	{
		layoff::IPC::ScopeLock lock(mutex);
		notifQueue.push(n);
		eventFire(&newData);
	}

}
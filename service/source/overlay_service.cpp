#include "overlay_service.hpp"
#include "../../source/IPC/ErrorCodes.h"

#include "Clients.hpp"
#include <nxIpc/Exceptions.hpp>

void PrintLn(const char* str) 
{
	services::OverlayService::PrintLn(std::string(str));
}

namespace services
{
	Event OverlayService::newData;
	std::atomic<bool> OverlayService::Locked;

	std::queue<std::string> OverlayService::printQueue;
	std::queue<IPCClient> OverlayService::clientQueue;
	std::queue<services::OverlayService::ClientUIPush> OverlayService::UIQueue;
	std::queue<SimpleNotification> OverlayService::notifQueue;

	bool OverlayService::ReceivedCommand(nxIpc::Request& req)
	{
		if (req.cmdId >= handlers.size())
		{
			nxIpc::Response(R_UNKNOWN_CMDID).Finalize();
			return true;
		}

		auto handler = handlers[req.cmdId];

		if (!handler)
		{
			nxIpc::Response(R_UNIMPLEMENTED_CMDID).Finalize();
			return true;
		}

		(this->*handler)(req);
		return false;
	}

	void OverlayService::InitializeStatics() 
	{
		Result rc = eventCreate(&newData, true);
		if (R_FAILED(rc))
			fatalThrow(rc);
	}

	void OverlayService::FinalizeStatics() 
	{
		eventClose(&newData);
	}

	void OverlayService::LockEvents(nxIpc::Request& req)
	{
		Locked = true;
		nxIpc::Response().Finalize();
	}

	void OverlayService::UnlockEvents(nxIpc::Request& req)
	{
		Locked = false;
		nxIpc::Response().Finalize();
	}

	void OverlayService::GetQueueStatus(nxIpc::Request& req)
	{
		IPCQUeueStatus res;
		res.Prints = printQueue.size();
		res.Notifs = notifQueue.size();
		res.Clients = clientQueue.size();
		res.UI = UIQueue.size();
		nxIpc::Response().Payload(res).Finalize();
	}

	void OverlayService::PopPrintQueue(nxIpc::Request& req)
	{
		auto& t = printQueue.front();

		auto buf = req.WriteBuffer(0);
		u32 toCopy = std::min(t.size(), buf.length);

		buf.AssignFrom_s(t.c_str(), toCopy);
		((char*)buf.data)[toCopy - 1] = '\0';
		
		nxIpc::Response().Payload<u32>(toCopy).Finalize();
		
		printQueue.pop();
	}

	void OverlayService::PopUIQueue(nxIpc::Request& req)
	{
		auto& t = UIQueue.front();
		auto buf = req.WriteBuffer(0);

		if (buf.length < t.data.size())
		{
			nxIpc::Response(ERR_BUFFER_TOO_SMALL).Finalize();
			return;
		}

		IPCUIPush res;
		res.client = t.client;
		res.BufferLen = t.data.size();
		res.header = t.header;
		buf.AssignFrom_s(t.data.data(), t.data.size());

		nxIpc::Response().Payload(res).Finalize();

		UIQueue.pop();
	}

	template<typename T>
	static inline u32 CopyToIPCBuf(std::queue<T>& queue, nxIpc::WritableBuffer& buf)
	{
		static_assert(std::is_pod<T>::value);

		int count = 0;
		u8* out = (u8*)buf.data;
		u8* limit = out + buf.length;
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

	void OverlayService::ReadClientQueue(nxIpc::Request& req)
	{		
		auto buf = req.WriteBuffer(0);
		u32 WrittenCount = CopyToIPCBuf(clientQueue, buf);
		nxIpc::Response().Payload<u32>(WrittenCount).Finalize();
	}

	void OverlayService::ReadNotifQueue(nxIpc::Request& req)
	{
		auto buf = req.WriteBuffer(0);
		u32 WrittenCount = CopyToIPCBuf(notifQueue, buf);
		nxIpc::Response().Payload<u32>(WrittenCount).Finalize();
	}

	void OverlayService::AcquireNewDataEvent(nxIpc::Request& req) {
		nxIpc::Response().CopyHandle(newData.revent).Finalize();
	}

	void OverlayService::PushUIStateChange(nxIpc::Request& req)
	{
		IPCUIEvent evt = *req.Payload<IPCUIEvent>();
		Result rc = layoff::IPC::clients::PushUIEventData(evt.Client, evt.evt);
		nxIpc::Response(rc).Finalize();
	}

	void OverlayService::PrintLn(const std::string&& str)
	{
		printQueue.push(std::move(str));
		eventFire(&newData);
	}

	void OverlayService::ClientAction(const IPCClient&& cli)
	{
		clientQueue.push(std::move(cli));
		eventFire(&newData);
	}
	
	void OverlayService::UIPush(const ClientUIPush&& p)
	{
		UIQueue.push(std::move(p));
		eventFire(&newData);
	}

	void OverlayService::NotifSimple(const SimpleNotification& n) 
	{
		notifQueue.push(n);
		eventFire(&newData);
	}

}
#pragma once
#include <layoff.h>
#include <queue>
#include <string>
#include <array>
#include <atomic>

#include <nxIpc/Types.hpp>
#include <nxIpc/Server.hpp>
#include <nxIpc/Exceptions.hpp>
#include "OverlayServiceTypes.h"

namespace services {
	class OverlayService : public nxIpc::IInterface {
	public:
		struct ClientUIPush
		{
			LayoffIdentifier client;
			LayoffUIHeader header;
			std::vector<u8> data;
		};
	private:
		using CallHandler = void (OverlayService::*)(nxIpc::Request& req);

		const std::array<CallHandler, 10> handlers{
			/* 0 */ nullptr,
			/* 1 */ &OverlayService::LockEvents,
			/* 2 */ &OverlayService::UnlockEvents,
			/* 3 */ &OverlayService::GetQueueStatus,
			/* 4 */ &OverlayService::PopPrintQueue,
			/* 5 */ &OverlayService::PopUIQueue,
			/* 6 */ &OverlayService::ReadClientQueue,
			/* 7 */ &OverlayService::ReadNotifQueue,
			/* 8 */ &OverlayService::AcquireNewDataEvent,
			/* 9 */ &OverlayService::PushUIStateChange
		};

		static Event newData;

		static std::queue<std::string> printQueue;
		static std::queue<IPCClient> clientQueue;
		static std::queue<services::OverlayService::ClientUIPush> UIQueue;
		static std::queue<SimpleNotification> notifQueue;
	public:
		static std::atomic<bool> Locked; //Not really needed as now server is single-threaded
		
		OverlayService()
		{
			Locked = false;
			LogFunction("Overlay svc created\n");
		}

		~OverlayService() 
		{
			LogFunction("Overlay svc destroyed\n");
		}

		static void InitializeStatics();
		static void FinalizeStatics();

		//Signaling
		static void PrintLn(const std::string&& str);
		static void ClientAction(const IPCClient&& cli);
		static void UIPush(const ClientUIPush&& p);
		static void NotifSimple(const SimpleNotification& n);

		bool ReceivedCommand(nxIpc::Request& req) override;
		//Reading		
		void LockEvents(nxIpc::Request& req);
		void UnlockEvents(nxIpc::Request& req);

		void GetQueueStatus(nxIpc::Request& req);

		//We can't know string len beforehand so we must pop strings one at a time
		void PopPrintQueue(nxIpc::Request& req);
		void PopUIQueue(nxIpc::Request& req);

		void ReadClientQueue(nxIpc::Request& req);
		void ReadNotifQueue(nxIpc::Request& req);

		void AcquireNewDataEvent(nxIpc::Request& req);

		//Writing
		void PushUIStateChange(nxIpc::Request& req);

	};
}
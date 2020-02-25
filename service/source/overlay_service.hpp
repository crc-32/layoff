#pragma once
#include <stratosphere.hpp>
#include <layoff.h>
#include <queue>
#include <string>

#include "OverlayServiceTypes.h"

namespace services {
	using namespace ams;

	class OverlayService : public ams::sf::IServiceObject {
	public:
		struct ClientUIPush
		{
			LayoffIdentifier client;
			LayoffUIHeader header;
			std::vector<u8> data;
		};
	private:
		enum class CommandId {
			LockEvents = LayoffOverlayCmdID_Lock,
			UnlockEvents = LayoffOverlayCmdID_Unlock,
			GetQueueStatus = LayoffOverlayCmdID_GetQueueStatus,
			PopPrintQueue = LayoffOverlayCmdID_PopPrintQueue,
			PopUIQueue = LayoffOverlayCmdID_PopUIQueue,
			ReadClientQueue = LayoffOverlayCmdID_ReadClientQueue,
			ReadNotifQueue = LayoffOverlayCmdID_ReadNotifQueue,
			AcquireNewDataEvent = LayoffOverlayCmdID_AcquireNewDataEvent,
			PushUIStateChange = LayoffOverlayCmdID_PushUIStateChange
		};						  

		static Event newData;
		static Mutex mutex;

		static std::queue<std::string> printQueue;
		static std::queue<IPCClient> clientQueue;
		static std::queue<services::OverlayService::ClientUIPush> UIQueue;
		static std::queue<SimpleNotification> notifQueue;
	public:
		static void InitializeStatics();

		OverlayService();
		~OverlayService();
	//Signaling
		static void PrintLn(const std::string&& str);
		static void ClientAction(const IPCClient&& cli);
		static void UIPush(const ClientUIPush&& p);
		static void NotifSimple(const SimpleNotification& n);

	//Reading
		ams::Result LockEvents();
		ams::Result UnlockEvents();

		ams::Result GetQueueStatus(sf::Out<IPCQUeueStatus> status);

		//We can't know string len beforehand so we must pop strings one at a time
		ams::Result PopPrintQueue(sf::OutBuffer buf, sf::Out<u32> WrittenLen);
		ams::Result PopUIQueue(sf::OutBuffer buf, sf::Out<IPCUIPush> data);
		
		ams::Result ReadClientQueue(sf::OutBuffer buf, sf::Out<u32> WrittenCount);
		ams::Result ReadNotifQueue(sf::OutBuffer buf, sf::Out<u32> WrittenCount);

		ams::Result AcquireNewDataEvent(sf::OutCopyHandle evt);

	//Writing
		ams::Result PushUIStateChange(IPCUIEvent evt);

		DEFINE_SERVICE_DISPATCH_TABLE{
			MAKE_SERVICE_COMMAND_META(LockEvents),
			MAKE_SERVICE_COMMAND_META(UnlockEvents),
			MAKE_SERVICE_COMMAND_META(GetQueueStatus),
			MAKE_SERVICE_COMMAND_META(PopPrintQueue),
			MAKE_SERVICE_COMMAND_META(PopUIQueue),
			MAKE_SERVICE_COMMAND_META(ReadClientQueue),
			MAKE_SERVICE_COMMAND_META(ReadNotifQueue),
			MAKE_SERVICE_COMMAND_META(AcquireNewDataEvent),
			MAKE_SERVICE_COMMAND_META(PushUIStateChange)
		};
	};

}
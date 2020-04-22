#include <cstring>

#include "layoff_service.hpp"
#include "../../source/IPC/ErrorCodes.h"
#include "Clients.hpp"
#include "overlay_service.hpp"

#include <nxIpc/Exceptions.hpp>

namespace services {
	LayoffService::LayoffService()
	{
		LogFunction("Layoff service created\n");
		id = layoff::IPC::clients::CreateIdentifier(this);
	}

	LayoffService::~LayoffService()
	{
		LogFunction("Layoff service destroyed\n");
		IPCClient action;
		action.id = id;
		action.action = OverlayAction_Disconnected;
		OverlayService::ClientAction(std::move(action));
		
		if (eventActive(&UiEvent))
			eventClose(&UiEvent);

		layoff::IPC::clients::FreeIdentifier(id);
	}

	bool LayoffService::ReceivedCommand(nxIpc::Request& req)
	{
		//TODO: either make layoff read all queues in a single call or figure out a safe way to do it
		//Not sure what happens if a module writes to a queue while OverlayService::Locked

		//if (OverlayService::Locked)
		//{
		//	nxIpc::Response(ERR_QUEUES_LOCKED).Finalize();
		//	return false;
		//}

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

	void LayoffService::PushUIEventData(const LayoffUIEvent& evt)
	{
		LastUIEvent = evt;
		if (eventActive(&UiEvent))
			eventFire(&UiEvent);
	}

	void LayoffService::SetClientName(nxIpc::Request &req) {		
		name = *req.Payload<LayoffName>();
		name.str[sizeof(name) - 1] = '\0';

		if (std::strlen(name.str) < 1)
		{
			nxIpc::Response(ERR_INVALID_NAME).Finalize();
			return;
		}

		IPCClient action;
		action.id = id;
		action.action = OverlayAction_Connected;
		std::memcpy(&action.name, &name, sizeof(LayoffName));
		OverlayService::ClientAction(std::move(action));

		nxIpc::Response().Finalize();
	}

	void LayoffService::NotifySimple(nxIpc::Request& req) {
		OverlayService::NotifSimple(*req.Payload<SimpleNotification>());
		nxIpc::Response().Finalize();
	}

	void LayoffService::NotifyEx(nxIpc::Request& req) {
		// TODO
		nxIpc::Response().Finalize();
	}

	void LayoffService::PushUIPanel(nxIpc::Request& req) {
		LayoffUIHeader header = *req.Payload<LayoffUIHeader>();
		
		if (header.panelID == 0)
		{
			nxIpc::Response(ERR_INVALID_ID).Finalize();
			return;
		}

		services::OverlayService::ClientUIPush push;
		push.client = id;
		push.header = header;

		if (header.kind != LayoffUIKind_None)
		{
			auto buf = req.ReadBuffer(0);
			push.data = std::vector<u8>((u8*)buf.data, (u8*)buf.data + buf.length);
		}
		
		OverlayService::UIPush(std::move(push));
		
		nxIpc::Response().Finalize();
		return;
	}

	void LayoffService::AcquireUiEvent(nxIpc::Request& req) {
		if (!eventActive(&UiEvent))
		{
			Result rc = eventCreate(&UiEvent, true);
			if (R_FAILED(rc))
			{
				nxIpc::Response(rc).Finalize();
				return;
			}
		}

		nxIpc::Response().CopyHandle(UiEvent.revent).Finalize();
	}

	void LayoffService::GetLastUiEvent(nxIpc::Request& req)
	{
		nxIpc::Response().Payload(this->LastUIEvent).Finalize();
		LastUIEvent = {};
	}

}
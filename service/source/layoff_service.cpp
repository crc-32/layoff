#include <cstring>

#include "layoff_service.hpp"
#include "../../source/IPC/ErrorCodes.h"
#include "Clients.hpp"

#include "overlay_service.hpp"

namespace services {
	LayoffService::LayoffService()
	{
		id = layoff::IPC::clients::CreateIdentifier(this);
	}

	LayoffService::~LayoffService()
	{
		IPCClient action;
		action.id = id;
		action.action = OverlayAction_Disconnected;
		OverlayService::ClientAction(std::move(action));
		
		if (eventActive(&UiEvent))
			eventClose(&UiEvent);
		layoff::IPC::clients::FreeIdentifier(id);
	}

	void LayoffService::PushUIEventData(const LayoffUIEvent& evt)
	{
		LastUIEvent = evt;
		if (eventActive(&UiEvent))
			eventFire(&UiEvent);
	}

	ams::Result LayoffService::SetClientName(LayoffName clientName) {		
		name = clientName;
		name.str[sizeof(name) - 1] = '\0';

		if (std::strlen(name.str) < 1)
			return ERR_INVALID_NAME;

		IPCClient action;
		action.id = id;
		action.action = OverlayAction_Connected;
		std::memcpy(&action.name, &name, sizeof(LayoffName));
		OverlayService::ClientAction(std::move(action));

		return ams::ResultSuccess();
	}

	ams::Result LayoffService::NotifySimple(SimpleNotification notification) {
		OverlayService::NotifSimple(notification);
		return ams::ResultSuccess();
	}

	ams::Result LayoffService::NotifyEx() {
		// TODO
		return ams::ResultSuccess();
	}

	ams::Result LayoffService::PushUIPanel(sf::InBuffer& buf, LayoffUIHeader header) {
		if (header.panelID == 0)
			return ERR_INVALID_ID;
		
		services::OverlayService::ClientUIPush push;
		push.client = id;
		push.header = header;
		push.data = std::vector<u8>(buf.GetPointer(), buf.GetPointer() + buf.GetSize());
		OverlayService::UIPush(std::move(push));
		return ams::ResultSuccess();
	}

	ams::Result LayoffService::AcquireUiEvent(sf::OutCopyHandle out_evt) {
		if (!eventActive(&UiEvent))
		{
			ams::Result rc = eventCreate(&UiEvent, true);
			if (rc.IsFailure())
				return rc;
		}

		*out_evt = UiEvent.revent;
		return ams::ResultSuccess();
	}

	ams::Result LayoffService::GetLastUiEvent(sf::Out<LayoffUIEvent> evt)
	{
		*evt = LastUIEvent;
		LastUIEvent = {};
		return ams::ResultSuccess();
	}

}
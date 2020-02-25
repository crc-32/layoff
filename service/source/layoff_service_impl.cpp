#include <stratosphere.hpp>
#include "layoff_service.hpp"
#include "Clients.hpp"
#include "overlay_service.hpp"
#include "../../source/IPC/ErrorCodes.h"

namespace services {

	LayoffService::LayoffService()
	{
		id = layoff::IPC::CreateClient();
	}

	LayoffService::~LayoffService()
	{
		IPCClient action;
		action.id = id;
		action.action = OverlayAction_Disconnected;
		OverlayService::ClientAction(std::move(action));
		
		layoff::IPC::RemoveClient(id);
	}

	ams::Result LayoffService::SetClientName(LayoffName clientName) {		
		ams::Result rc = layoff::IPC::SetClientName(id, clientName);
		if (rc.IsFailure())
			return rc;

		IPCClient action;
		action.id = id;
		action.action = OverlayAction_Connected;
		std::memcpy(&action.name, &clientName, sizeof(LayoffName));
		OverlayService::ClientAction(std::move(action));

		return rc;
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

	ams::Result LayoffService::AcquireUiEvent(sf::OutCopyHandle evt) {
		return layoff::IPC::GetClientUIEventHandle(id, evt.GetHandlePointer());
	}

	ams::Result LayoffService::GetLastUiEvent(sf::Out<LayoffUIEvent> evt)
	{
		return layoff::IPC::GetClientUIEvent(id, evt.GetPointer());
	}

}
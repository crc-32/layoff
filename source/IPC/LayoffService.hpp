#pragma once
#include <stratosphere.hpp>
#include "../utils.hpp"
#include <layoff.h>
#include "../ConsoleStatus.hpp"
#include <sstream>

#include "../NotificationManager.hpp"
#include "Clients.hpp"

namespace services {

	using namespace ams;

	class LayoffService : public ams::sf::IServiceObject {

	private:
		enum class CommandId {
			SetClientName = LayoffCmdId_SetClientName,
			NotifySimple = LayoffCmdId_NotifySimple,
			NotifyEx = LayoffCmdId_NotifyEx,
			PushUIPanel = LayoffCmdId_PushUIPanel,
			AcquireUiEvent = LayoffCmdId_AcquireUiEvent
		};

		LayoffIdentifier id;
	public:
		LayoffService()
		{
			id = layoff::IPC::CreateClient();
		}

		~LayoffService()
		{
			layoff::IPC::RemoveClient(id);
		}

		ams::Result SetClientName(LayoffName clientName) {
			return layoff::IPC::SetClientName(id, clientName);
		}

		ams::Result NotifySimple(SimpleNotification notification) {
			layoff::notif::PushSimple(notification.message, notification.name.str);
			return ams::ResultSuccess();
		}

		ams::Result NotifyEx() {
			// TODO
			return ams::ResultSuccess();
		}
		
		ams::Result PushUIPanel(sf::InBuffer& buf, LayoffUIHeader header) {
			return layoff::IPC::AddUIPanel(id, header, buf.GetPointer(), buf.GetSize());
		}

		ams::Result AcquireUiEvent(sf::Out<LayoffUIEvent> evt) {
			return layoff::IPC::GetClientUIEvent(id, evt.GetPointer());
		}

		DEFINE_SERVICE_DISPATCH_TABLE{
			MAKE_SERVICE_COMMAND_META(NotifySimple),
			MAKE_SERVICE_COMMAND_META(NotifyEx),
			MAKE_SERVICE_COMMAND_META(SetClientName),
			MAKE_SERVICE_COMMAND_META(PushUIPanel),
			MAKE_SERVICE_COMMAND_META(AcquireUiEvent),
		};
	};

}
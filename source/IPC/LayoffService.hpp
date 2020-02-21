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
                NotifySimple = LayoffCmdId_NotifySimple,
                NotifyEx = LayoffCmdId_NotifyEx,
				RegisterClient = LayoffCmdId_RegisterClient,
				UnregisterClient = LayoffCmdId_UnregisterClient,
				PushUIPanel = LayoffCmdId_PushUIPanel,
				//GetUIState = LayoffCmdId_GetUIState,
				AcquireUiEvent = LayoffCmdId_AcquireUiEvent 
            };

        public:
            ams::Result NotifySimple(SimpleNotification notification) {
				layoff::notif::PushSimple(notification.message, notification.name.name);
				return ams::ResultSuccess();
            }

			ams::Result NotifyEx() {
				// TODO
				return ams::ResultSuccess();
			}

			ams::Result RegisterClient(sf::Out<LayoffIdentifier> id, LayoffName clientName) {
				*id = layoff::IPC::CreateClient(clientName.name);
				return ams::ResultSuccess();
			}
			
			ams::Result UnregisterClient(LayoffIdentifier id) {
				return layoff::IPC::RemoveClient(id);
			}
			
			ams::Result PushUIPanel(sf::InBuffer &buf, LayoffUIHeader header) {
				return layoff::IPC::AddUIPanel(header, buf.GetPointer(), buf.GetSize());
			}

			ams::Result AcquireUiEvent(sf::Out<LayoffUIEvent> evt, LayoffIdentifier client) {
				return layoff::IPC::GetClientUIEvent(client, evt.GetPointer());
			}

            DEFINE_SERVICE_DISPATCH_TABLE {
                MAKE_SERVICE_COMMAND_META(NotifySimple),
				MAKE_SERVICE_COMMAND_META(NotifyEx),
				MAKE_SERVICE_COMMAND_META(RegisterClient),
				MAKE_SERVICE_COMMAND_META(UnregisterClient),
				MAKE_SERVICE_COMMAND_META(PushUIPanel),
				MAKE_SERVICE_COMMAND_META(AcquireUiEvent),
            };
    };

}
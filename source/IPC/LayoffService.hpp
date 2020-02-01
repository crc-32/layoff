#pragma once
#include <stratosphere.hpp>
#include "../utils.hpp"
#include <layoff.h>
#include "../ConsoleStatus.hpp"
#include <sstream>

namespace services {

    class LayoffService : public ams::sf::IServiceObject {

        private:
            enum class CommandId {
                NotifySimple = LayoffCmdId_NotifySimple,
                NotifyEx = LayoffCmdId_NotifyEx,
            };

        public:
            ams::Result NotifySimple(SimpleNotification notification) {
                // TODO
                layoff::console::UpdateStatus();
                auto s = &layoff::console::Status.Timestamp;
                std::stringstream timestamp;
                timestamp << s;
                layoff::nman->PushNotif(notification.message, timestamp.str(), notification.identifier);
                return ams::ResultSuccess();
            }

            ams::Result NotifyEx() {
                // TODO
                return ams::ResultSuccess();
            }

            DEFINE_SERVICE_DISPATCH_TABLE {
                MAKE_SERVICE_COMMAND_META(NotifySimple),
                MAKE_SERVICE_COMMAND_META(NotifyEx),
            };
    };

}
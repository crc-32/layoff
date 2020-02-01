#pragma once

#include <string>
#include <map>
#include "UI/Notification.hpp"

namespace layoff {
    class NotificationManager
    {
        public:
        void PushNotif(std::string content, std::string notifIdentifier, std::string ipcIdentifier) ;
        void PopNotif(std::string notifIdentifier, std::string ipcIdentifier);
        void Update();
        private:
        std::map<std::string, UI::Notification*> notifications;
    };
}
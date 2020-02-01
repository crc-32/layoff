#include "NotificationManager.hpp"
#include <sstream>
#include <algorithm>

namespace layoff {
    void NotificationManager::PushNotif(std::string content, std::string notifIdentifier, std::string ipcIdentifier) {
        std::stringstream fullid;
        fullid << notifIdentifier << "-" << ipcIdentifier;
        std::string fid = fullid.str();

        this->notifications.insert_or_assign(fid, new UI::Notification(content, fid, 4, false));
    }

    void NotificationManager::PopNotif(std::string notifIdentifier, std::string ipcIdentifier) {
        std::stringstream fullid;
        fullid << notifIdentifier << "-" << ipcIdentifier;
        std::string fid = fullid.str();
        delete this->notifications[fid];
        this->notifications.erase(fid);
    }

    void NotificationManager::Update() {
        for( auto const& [key, val] : notifications )
        {
            if (!val->Update()) {
                delete val;
                notifications.erase(key);
            }
        }
    }
}
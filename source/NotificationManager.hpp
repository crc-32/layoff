#pragma once
#include <deque>
#include "Notification.hpp"
#include "VolumeNotification.hpp"
using namespace std;

class NotificationManager
{
    public:
        NotificationManager();
        void Render();
        void Push(string id, string contentText, string iconPath, u32 timeout);
        void Pop();
        void PopAll();
        void ShowLatest();
        void ShowID(string id);
        bool IDInUse(string id);
        void HideID(string id);
        bool IsActive();
        void HandleVolume(u16 step);

    private:
        deque<Notification*> notifications;
        VolumeNotification *volNotif;
};
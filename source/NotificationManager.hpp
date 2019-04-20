#pragma once
#include <deque>
#include "Notification.hpp"
#include "VolumeNotification.hpp"
#include "screenConsole.hpp"
using namespace std;

class NotificationManager
{
    public:
        NotificationManager(ScreenConsole *console);
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
        void EventHandler(u32 batteryPercentage);

    private:
        deque<Notification*> notifications;
        VolumeNotification *volNotif;
        Event notifEvent;
        ScreenConsole *console;
};
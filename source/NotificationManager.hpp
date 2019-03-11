#pragma once
#include <deque>
#include "Notification.hpp"
using namespace std;

class NotificationManager
{
    public:
        void Render();
        void Push(string id, string headerText, string iconPath, u32 timeout);
        void Pop();
        void PopAll();
        void ShowLatest();
        void ShowID(string id);
        bool IDInUse(string id);
        void HideID(string id);
        bool IsActive();

    private:
        deque<Notification*> notifications;
};
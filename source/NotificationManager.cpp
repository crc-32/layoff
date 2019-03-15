#include "NotificationManager.hpp"
#include "UI/imgui_sdl.h"

void NotificationManager::Render()
{
    if (!this->notifications.empty()){
        if (!this->notifications.front()->Draw(0))
            this->notifications.pop_front();
    }
}

void NotificationManager::Push(string id, string contentText, string iconPath, u32 timeout)
{
    if(this->IDInUse(id))
    {
        for(size_t i = 0; i < this->notifications.size()-1; i++)
        {
            if(this->notifications[i]->GetID() == id){
                this->notifications[i]->SetContentText(contentText);
                this->notifications[i]->SetIcon((iconPath != "") ? ImGuiSDL::LoadTexture(iconPath.c_str()) : NULL);
                break;
            }
        }
    }else{
        this->notifications.push_front(new Notification(id, contentText, (iconPath != "") ? ImGuiSDL::LoadTexture(iconPath.c_str()) : NULL, timeout));
    }

}

void NotificationManager::Pop()
{
    if(this->notifications.front())
        this->notifications.pop_front();
}

void NotificationManager::PopAll()
{
    while(!this->notifications.empty())
        this->notifications.pop_front();
}

void NotificationManager::ShowLatest()
{
    this->notifications.front()->Show();
}

void NotificationManager::ShowID(string id)
{
    for(size_t i = 0; i < this->notifications.size()-1; i++)
    {
        if(this->notifications[i]->GetID() == id){
            this->notifications[i]->Show();
        }
    }
}

bool NotificationManager::IDInUse(string id)
{
    if(this->notifications.empty()) return false;
    for(size_t i = 0; i < this->notifications.size()-1; i++)
    {
        if(this->notifications[i]->GetID() == id){
            return true;
        }
    }
    return false;
}

void NotificationManager::HideID(string id)
{
    if(this->notifications.empty()) return;
    for(size_t i = 0; i < this->notifications.size()-1; i++)
    {
        if(this->notifications[i]->GetID() == id){
            this->notifications[i]->Hide();
        }
    }
}

bool NotificationManager::IsActive()
{
    return !this->notifications.empty();
}
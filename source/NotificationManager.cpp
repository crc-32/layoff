#include "NotificationManager.hpp"
//#include "UI/imgui_sdl.h"
#include "screenConsole.hpp"
#include <string>
#include <iomanip>

NotificationManager::NotificationManager(ScreenConsole *console)
{
    this->volNotif = nullptr;
    ovlnIReceiverGetEvent(&this->notifEvent);
    this->console = console;
}

void NotificationManager::EventHandler(u32 batteryPercentage)
{
	if(!eventActive(&this->notifEvent))
			fatalSimple(MAKERESULT(255, 321));
	if(!R_FAILED(eventWait(&this->notifEvent, 1000000)))
	{
		IReceiverNotification n;
		ovlnIReceiverGetNotification(&n);
		std::stringstream print;
		std::stringstream nText;
		switch (n.type)
		{
			case BatteryNotifType:
				print << "Charge notif\n";
				ChargerType cType;
				psmGetChargerType(&cType);
				if(cType != ChargerType_None)
				{
					this->HideID("batlow");
					if(batteryPercentage != 100)
					{
						nText << "Charging " << to_string(batteryPercentage) << "%%";
						this->Push("charge", nText.str(), "romfs:/notificationIcons/batCharge.png", 5);
					}else{
						nText << "Charged";
						this->Push("charge", nText.str(), "romfs:/notificationIcons/batCharged.png", 5);
					}
				}else{
					this->HideID("charge");
					if(batteryPercentage <= 15)
						this->Push("batlow", nText.str(), "romfs:/notificationIcons/batLow.png", 0);
				}
				break;
			case VolumeNotifType:
				print << "Volume notif:" << to_string(n.content) << "\n";
				this->HandleVolume(n.content);
				break;
			case ScreenshotNotifType:
				print << "Screenshot notif\n";
				nText << "Capture taken";
				this->Push("scrtaken", nText.str(), "romfs:/notificationIcons/screenshot.png", 3);
				break;
			case ScreenshotFailNotifType:
				print << "Screenshot fail notif\n";
				nText << "Capture failed";
				this->Push("scrfail", nText.str(), "romfs:/notificationIcons/screenshot.png", 3);
				break;
			case VideoNotifType:
				print << "Video notif\n";
				nText << "Video capture taken";
				this->Push("vidtaken", nText.str(), "romfs:/notificationIcons/video.png", 3);
				break;
			case VideoFailNotifType:
				print << "Video fail notif\n";
				nText << "Video capture failed";
				this->Push("vidfail", nText.str(), "romfs:/notificationIcons/video.png", 3);
				break;
			default:
				print << "Unknown notif:" << std::hex << n.type << "\n";
				break;
		}
		console->Print(print.str());
	}
}

void NotificationManager::Render()
{
    if (!this->notifications.empty()){
        if (!this->notifications.front()->Draw(0))
            this->notifications.pop_front();
    }
    if(volNotif)
    {
        if(!volNotif->Draw(0))
            volNotif = nullptr;
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
                //this->notifications[i]->SetIcon((iconPath != "") ? ImGuiSDL::LoadTexture(iconPath.c_str()) : NULL);
                this->notifications[i]->Show();
                break;
            }
        }
    }else{
        //this->notifications.push_front(new Notification(id, contentText, (iconPath != "") ? ImGuiSDL::LoadTexture(iconPath.c_str()) : NULL, timeout));
        this->notifications.push_front(new Notification(id, contentText, /*NULL,*/ timeout));
        this->ShowLatest();
    }

}

void NotificationManager::HandleVolume(u16 step)
{
    if(volNotif)
    {
        volNotif->setStep(step);
        volNotif->Show();
    }else
    {
        volNotif = new VolumeNotification(step);
        volNotif->Show();
    }
}

void NotificationManager::Pop()
{
    if(this->notifications.front())
    {
        renderDirty = 3;
        this->notifications.pop_front();
    }
}

void NotificationManager::PopAll()
{
    while(!this->notifications.empty())
    {
        renderDirty = 3;
        this->notifications.pop_front();
    }
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
    return !this->notifications.empty() || this->volNotif != nullptr;
}
#pragma once
#include <switch.h>
#include "Notification.hpp"
using namespace std;

class VolumeNotification : public Notification
{
public:
    VolumeNotification(u16 currentStep) : Notification("volume", "", NULL, 3)
    {
        this->setStep(currentStep);
    }

    void setStep(u16 step)
    {
        this->currentStep = step;
        if (step != 0 && step < 256)
            this->SetIcon(ImGuiSDL::LoadTexture("romfs:/notificationIcons/fullIcon.png"));
        else
            this->SetIcon(ImGuiSDL::LoadTexture("romfs:/notificationIcons/muteIcon.png"));
        timeCreated = time(NULL);
    }

    bool Draw(float yPos)
    {
        if (!isVisible) return false;
        u64 timeNow = time(NULL);
        if (timeNow - timeCreated >= timeout && timeout != 0)
        {
            this->Hide();
            return false;
        }
        if(!ImGui::Begin(id.c_str(), NULL, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav))
        {
            ImGui::End();
            return false;
        }
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0,0,0,20));
        ImGui::SetWindowSize(ImVec2(420, 90));
        ImGui::SetWindowPos(ImVec2(0, yPos));
        if(icon)
        {
            ImGui::SetCursorPos(ImVec2(8, 13));
            ImGui::Image(icon, ImVec2(64,64));
        }
        ImGui::SetCursorPos(ImVec2(8+64+4, 45-(36/2)));
        if(currentStep < 256)
        {
            ImGui::ProgressBar(((float)currentStep)/15.0f, ImVec2((420-(8+64+4))-8, 36));
        }else{ // Full muted
            ImGui::ProgressBar(0.0f/15.0f, ImVec2((420-(8+64+4))-8, 36), "Mute");
        }
        ImGui::PopStyleColor();
        ImGui::End();
        return true;
    }
private:
    u16 currentStep;
};
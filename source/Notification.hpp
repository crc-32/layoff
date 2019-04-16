#pragma once
#include <switch.h>
#include <time.h>
#include <string>
#include "UI/UI.hpp"
#include "UI/imgui_sdl.h"
using namespace std;

class Notification
{
    public:
        Notification(string id, string contentText, Texture *icon, u32 timeout)
        {
            this->contentText = contentText;
            this->icon = icon;
            this->isVisible = false;
            this->timeout = timeout;
            this->id = id;
            timeCreated = 0;
        }

        void SetContentText(string contentText)
        {
            this->contentText = contentText;
        }

        void SetIcon(Texture *icon)
        {
            if(this->icon)
                ImGuiSDL::FreeTexture(this->icon);
            this->icon = icon;
        }
        
        ~Notification()
        {
            if(icon){
                ImGuiSDL::FreeTexture(icon);
            }
        }

        void Show()
        {
            this->isVisible = true;
            timeCreated = time(NULL);
        }

        void Hide()
        {
            this->isVisible = false;
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
            ImGui::SetCursorPos(ImVec2(8+64+4, 45-(ImGui::CalcTextSize(contentText.c_str()).y/2)));
            ImGui::Text(contentText.c_str());
            ImGui::PopStyleColor();
            ImGui::End();
            return true;
        }

        string GetID()
        {
            return id;
        }

    private:
        string contentText;
    
    protected:
        string id;
        bool isVisible;
        u32 timeout;
        u64 timeCreated;
        Texture *icon;
};
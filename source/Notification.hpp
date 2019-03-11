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
        Notification(string id, string headerText, Texture *icon, u32 timeout)
        {
            this->headerText = headerText;
            this->icon = icon;
            this->isVisible = false;
            this->timeout = timeout;
            this->id = id;
            timeCreated = 0;
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
            if (timeNow - timeCreated >= timeout)
            {
                this->Hide();
                return false;
            }
            if(!ImGui::Begin(headerText.c_str(), NULL, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs))
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
            ImGui::SetCursorPos(ImVec2(8+64+4, 45-(ImGui::CalcTextSize(headerText.c_str()).y/2)));
            ImGui::Text(headerText.c_str());
            ImGui::PopStyleColor();
            ImGui::End();
            return true;
        }

        string GetID()
        {
            return id;
        }

    private:
        string headerText;
        Texture *icon;
        bool isVisible;
        u32 timeout;
        u64 timeCreated;
        string id;
};
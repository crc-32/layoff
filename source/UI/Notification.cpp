#include "Notification.hpp"
#include "../utils.hpp"
#include <string>

using namespace layoff::UI;

void Notification::DoUpdate()
{
    ImGui::SetCursorPos(ImVec2(5, 90/2 - ImGui::CalcTextSize(this->content.c_str()).y/2));
    ImGui::Text(this->content.c_str());
}

Notification::Notification(std::string content, std::string identifier, long expirySec, bool playSound)
{
    this->identifier = identifier;
    this->content = content;
    if (expirySec == -1) {
        this->expiry = -1;
    }else {
        this->expiry = std::time(nullptr) + expirySec;
    }
    if (playSound)
    {
        PrintLn("W: playSound req but not implemented");
    }
}
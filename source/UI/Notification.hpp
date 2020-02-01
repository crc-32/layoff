#pragma once

#include <imgui/imgui.h>
#include "rendering/imgui_sw.hpp"
#include "Window.hpp"
#include <ctime>

namespace layoff::UI {
	
	class Notification
	{	
	public:
        std::string identifier;
		bool Update()
		{
            if (this->expiry - std::time(nullptr) <= 0 && this->expiry != -1){
                return false;
            }
			PushStyling();
			DoUpdate();
			PopStyling();
            return true;
		}

		~Notification()
		{
			
		}

        Notification(std::string content, std::string identifier, long expirySec, bool playSound);

	private: 
        std::string content;
        std::time_t expiry;

		inline void PushStyling()
		{
			ImGui::Begin(identifier.c_str(), 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus);
			ImGui::SetWindowPos({0, 0});
			ImGui::SetWindowSize({450, 90});
		}
		
		inline void PopStyling()
		{
			ImGui::End();
		}

		void DoUpdate();
	};

}
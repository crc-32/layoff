#if LAYOFF_LOGGING
#pragma once

#include <imgui/imgui.h>
#include "../utils.hpp"
#include "Window.hpp"
#include <string>

static std::string LogText = "";

namespace layoff::UI {
	
	class LogWindow : public Window
	{	
	public: 
		bool Visible = true;
		void RequestClose() override 
		{
			Visible = false;
		}
	
		void Update() override 
		{
			ImGui::Begin("Log window", &Visible);
			ImGui::SetWindowSize(ImVec2(300, 400), ImGuiCond_Once);
			ImGui::Text(LogText.c_str());
			ImGui::End();
		}
		
		bool ShouldRender() override
		{
			return Visible;
		}
		
		~LogWindow() override 
		{
			
		}
	};

}

void PrintLn(const std::string& s)
{
	LogText += s + "\n";
}

void Print(const std::string& s)
{
	LogText += s;
}
#endif
#pragma once

#include <imgui/imgui.h>
#include "../utils.hpp"
#include "Window.hpp"

namespace layoff::UI {
	
	class PowerWindow : public Window
	{	
	public:
		bool Visible = true;
		void RequestClose() override 
		{
			Visible = false;
		}
	
		void Update() override 
		{
			PushStyling();
			DoUpdate();
			PopStyiling();
			
			if (BackPressed()) 
				Visible = false;
		}
		
		bool ShouldRender() override
		{
			return Visible;
		}
		
		~PowerWindow() override 
		{
			
		}
	private: 
		inline void PushStyling()
		{
			ImGui::Begin("PowerWindow", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
			ImGui::SetWindowPos({0, 0});
			ImGui::SetWindowSize({1280, 720});
		}
		
		inline void PopStyiling()
		{
			ImGui::End();
		}
		
		//Implemented in .cpp cause requires a lot of code
		void DoUpdate();
		
		bool PayloadInitFail = false;
	};

}
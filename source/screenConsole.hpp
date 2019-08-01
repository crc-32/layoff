#pragma once
#include "UI/UI.hpp"
//#include "UI/imgui_sdl.h"
#include <string>

class ScreenConsole
{
public:
	ScreenConsole()
	{
		//dbgTex = ImGuiSDL::LoadTexture("romfs:/trollface.png");
	}
	
	~ScreenConsole()
	{
		//ImGuiSDL::FreeTexture(dbgTex);
		//dbgTex = nullptr;
	}

	std::string Text = "";
	
	void Print(const std::string &str)
	{
		Text += str;
	}

	bool Display = true;
	bool Draw()
	{
		if (!Display) return false;
		if (!ImGui::Begin("Screen console"))
		{
			ImGui::End();
			return false;
		}
		//ImGui::Image(dbgTex, dbgTex->Size());
		ImGui::Text(Text.c_str());
		ImGui::End();
		return true;
	}
	
private:
	//Texture *dbgTex;
};
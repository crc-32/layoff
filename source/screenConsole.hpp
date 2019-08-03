#pragma once
#include "UI/UI.hpp"
//#include "UI/imgui_sdl.h"
#include <string>

class ScreenConsole
{
public:

	std::string Text = "";
	
	void Print(const std::string &str)
	{
		Text += str;
		renderDirty = 3;
	}

	bool Display = true;
	bool Draw()
	{
		if (!Display) return false;
		if (renderDirty > 0)
		{
		if (!ImGui::Begin("Screen console"))
		{
			ImGui::End();
			return false;
		}
		ImGui::Text(Text.c_str());
		ImGui::End();
		}
		return true;
	}
};
#pragma once

#include "../../ConsoleStatus.hpp"
#include "../../set/ConsoleSettings.hpp"

#define CONSOLE_STATUS_SHORTCUT auto st = &layoff::console::Status;
namespace layoff::UI::sidebar
{
	static inline void BrightnessControl()
	{
		CONSOLE_STATUS_SHORTCUT

		if (ImGui::Checkbox("Auto brightness", &st->AutoBrightness))
			layoff::set::SetAutoBrightness(st->AutoBrightness);

		ImGui::Spacing();

		ImGui::Text("Brightness "); ImGui::SameLine();
		if (ImGui::SliderFloat("##Brightness", &st->BrightnessLevel, 0.0f, 1.0f, ""))
			layoff::set::SetBrightness(st->BrightnessLevel);

		ImGui::NewLine();
	}
	
	static inline void WirelessControl()
	{		
		CONSOLE_STATUS_SHORTCUT
		
		if (st->Connected())
			ImGui::Text("Ip address: %s",st->IpStr);
		else 
			ImGui::Text("Not connected");
		
		ImGui::Spacing();
		
		if (ImGui::Button(st->WirelessEnabled ? "Disable wireless" : "Enable wireless", ImVec2(511, 0)))
			layoff::set::SetWireless(!st->WirelessEnabled);

		ImGui::NewLine();
	}
	
}
#undef CONSOLE_STATUS_SHORTCUT
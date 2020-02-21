#pragma once

#include "../../ConsoleStatus.hpp"
#include "../../set/ConsoleSettings.hpp"
#include "Sidebar.hpp"

#define CONSOLE_STATUS_SHORTCUT auto st = &layoff::console::Status;
namespace layoff::UI::sidebar
{
	static inline void BrightnessControl()
	{
		CONSOLE_STATUS_SHORTCUT

			ImGui::TextUnformatted("Brightness ");

		ImGui::SameLine();

		ImGui::PushItemWidth(250);
		if (ImGui::SliderFloat("##Brightness", &st->BrightnessLevel, 0.0f, 1.0f, ""))
			layoff::set::SetBrightness(st->BrightnessLevel);

		ImGui::SameLine(Sidebar::W - 100, 0);

		if (ImGui::Checkbox("Auto", &st->AutoBrightness))
			layoff::set::SetAutoBrightness(st->AutoBrightness);
	}

	static inline void WirelessControl()
	{
		CONSOLE_STATUS_SHORTCUT

			if (st->Connected())
				ImGui::Text("Ip: %s", st->IpStr);
			else
				ImGui::Text("Not connected");

		ImGui::SameLine(Sidebar::W - 210, 0);

		if (ImGui::Button(st->WirelessEnabled ? "Disable wireless" : "Enable wireless", ImVec2(200, 0)))
			layoff::set::SetWireless(!st->WirelessEnabled);
	}

}
#undef CONSOLE_STATUS_SHORTCUT
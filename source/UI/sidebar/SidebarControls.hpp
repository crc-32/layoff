#pragma once

#include "../../ConsoleStatus.hpp"

#define CONSOLE_STATUS_SHORTCUT auto st = &layoff::console::Status;
namespace layoff::UI::sidebar
{
	static inline void BrightnessControl()
	{
		CONSOLE_STATUS_SHORTCUT
		
		if (ImGui::Checkbox("Auto brightness", &st->AutoBrightness))
		{
			if (st->AutoBrightness)
			{
				if (R_FAILED(lblEnableAutoBrightnessControl()))
					Print("Auto brightness change failed\n");
			}
			else
			{
				if (R_FAILED(lblDisableAutoBrightnessControl()))
					Print("Auto brightness change failed\n");
			}
			layoff::console::RequestStatusUpdate();
		}
		ImGui::Spacing();
		ImGui::Text("Brightness "); ImGui::SameLine();
		if (ImGui::SliderFloat("##Brightness", &st->BrightnessLevel, 0.0f, 1.0f, ""))
			if (R_FAILED(lblSetCurrentBrightnessSetting(st->BrightnessLevel)))
			{
				Print("Brightness change failed\n");				
				layoff::console::RequestStatusUpdate();
			}			
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
		
		if (st->WirelessEnabled)
		{
			if (ImGui::Button("Disable wireless", ImVec2(511, 0)))
			{
				nifmSetWirelessCommunicationEnabled(false);
				nifmIsWirelessCommunicationEnabled(&st->WirelessEnabled);
			}
		}
		else if (ImGui::Button("Enable wireless", ImVec2(511, 0)))
		{
			nifmSetWirelessCommunicationEnabled(true);
			nifmIsWirelessCommunicationEnabled(&st->WirelessEnabled);
		}
		ImGui::NewLine();
	}
	
	
}
#undef CONSOLE_STATUS_SHORTCUT
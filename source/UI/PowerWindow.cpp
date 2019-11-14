#include "PowerWindow.hpp"
#include "../utils.hpp"
#include "../IPC/GeneralChannel.hpp"

using namespace layoff::UI;
using namespace layoff::ipc;

void PowerWindow::DoUpdate()
{
	ImGui::SetCursorPosY(100);
	ImGui::SetCursorPosX((1280/2)-(500/2)); // Pos = (Screen size/2) - (Btn size/2) = Center
	
	if(ImGui::Button("Sleep Mode", ImVec2(500,78))){
		qlaunch::PowerSleep();
		Visible = false;
		return;
	}
	
	ImGui::NewLine();
	ImGui::SetCursorPosX((1280/2)-(500/2));
	
	if(ImGui::Button("Reboot", ImVec2(500,78)))
		qlaunch::PowerReboot();
	
	ImGui::SetCursorPosX((1280/2)-(500/2));
	
	if(ImGui::Button("Power off", ImVec2(500,78)))
		qlaunch::PowerShutdown();
	
	ImGui::NewLine();
	ImGui::SetCursorPosX((1280/2)-(500/2));
	
	if(ImGui::Button("Close", ImVec2(500,78)))
		Visible = false;
}

#include "Sidebar.hpp"

#include "../rendering/ImguiExt.hpp"
#include "../../utils.hpp"
#include "SidebarControls.hpp"
#include <string>
#include "../UI.hpp"

#include "../../IPC/Clients.hpp"

#include <imgui/imgui_internal.h>

using namespace layoff;
using namespace layoff::UI;

void Sidebar::RequestClose()
{
	Visible = false;
}

void Sidebar::Update()
{
	PushStyling();
	DoUpdate();
	PopStyiling();
}

void Sidebar::FocusSidebar() 
{
	ImGui::FocusWindow(ImGui::FindWindowByName("sidebar"));
	ImGui::SetNavID(ImGui::FindWindowByName("sidebar")->ID, 0);
}

bool Sidebar::HighFreqUpdate()
{
	//for (WinPtr& win : Windows)
	//	if (win->HighFreqUpdate())
	//		return true;
	return false;
}

bool Sidebar::ShouldRender()
{
	return Visible;
}

inline void Sidebar::PushStyling()
{
	ImGui::Begin("sidebar", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos({ 1280 - W, 0 });
	ImGui::SetWindowSize({ Sidebar::W, Sidebar::H });
}

inline void Sidebar::PopStyiling()
{
	ImGui::End();
}

inline void Sidebar::DoUpdate()
{
	auto s = &console::Status.DateTime;
	ImGui::PushFont(Font30);
	ImGui::Text("%d:%d   %d/%d", s->hour, s->minute, s->day, s->month);
	ImGui::SameLine();
	ImGui::TextRight("%d%%", console::Status.BatteryLevel);
	ImGui::PopFont();
	ImGui::Spacing();

	ImGui::BeginChild("WidgetsArea", { W - 5, H * 3 / 4 });
	
	{
		//Have clients go out of scope as soon as possible
		auto&& clients = layoff::IPC::LockClients();
		for (auto& cli : clients.obj)
		{
			if (cli.second.Panels.size() == 0) continue;
			if (ImGui::CollapsingHeader(cli.second.name, ImGuiTreeNodeFlags_DefaultOpen)) {

				for (auto& control : cli.second.Panels)
				{
					control.second->Update();
					if (control.second->SignalEvent())
						cli.second.LastUIEvent = control.second->GetEvent();
				}
			}
		}
	}

	ImGui::EndChild();

	if (BackPressed())
	{
		if (ImGui::IsItemFocused())
			FocusSidebar();
		else
			Visible = false;
	}

	ImGui::NewLine();

	sidebar::WirelessControl();
	ImGui::Spacing();
	sidebar::BrightnessControl();
}
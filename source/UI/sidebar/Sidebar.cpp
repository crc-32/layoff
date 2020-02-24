#include "Sidebar.hpp"
#include "SidebarControls.hpp"

#include "../../utils.hpp"
#include "../UI.hpp"
#include "../../IPC/Clients.hpp"

#include <imgui/imgui_internal.h>
#include <string>

using namespace layoff;
using namespace layoff::UI;

void Sidebar::RequestClose()
{
	Visible = false;
}

void Sidebar::Update()
{
	BackgroundWindow();

	PushStyling();
	DoUpdate();
	PopStyiling();
}

void Sidebar::BackgroundWindow() 
{
	//This is used to detect taps on empty parts of the screen and close the overlay
	auto sz = ImVec2{ DEFAULT_WIN_WIDTH - Sidebar::W, 720 };

	ImGui::Begin("bgWin", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing);
	ImGui::SetWindowPos({ 0, 0 });
	ImGui::SetWindowSize(sz);

	ImGui::SetCursorPos({ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0,0,0,0 });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0,0,0,0 });

	if (ImGui::Button("##close", sz))
		Visible = false;

	ImGui::PopStyleColor(3);
	ImGui::End();
}

void Sidebar::Focus() 
{
	ImGui::SelectItem(true, ImGui::GetID("###WirelessBtn"));
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
	ImGui::Text("%d:%02d   %d/%d   ", s->hour, s->minute, s->day, s->month);
	ImGui::SameLine();
	
	//TODO: replace these with images
	if (!console::Status.Connected())
		ImGui::Text("[Offline]");
	else if (console::Status.connectionType == NifmInternetConnectionType_Ethernet)
		ImGui::Text("[Ethernet]");
	else if (console::Status.connectionType == NifmInternetConnectionType_WiFi)
	{ 
		char bars[] = "---";
		for (u32 i = 0; i < console::Status.ConnectionStrenght && i < 3; i++)
			bars[i] = '|';
		ImGui::Text("[Wifi %s]", bars);
	}
	ImGui::SameLine();
	
	ImGui::TextRight("%d%%%s", console::Status.BatteryLevel, console::Status.chargerType == ChargerType_None ? "" : "+");
	ImGui::PopFont();
	ImGui::Spacing();

	ImGui::BeginChild("WidgetsArea", { W - 5, H * 3 / 4 });
	
	{
		//Have clients go out of scope as soon as possible
		auto&& clients = layoff::IPC::LockClients();
		for (auto& cli : clients.obj)
		{
			if (cli.second.Panels.size() == 0) continue;
			if (ImGui::CollapsingHeader(cli.second.name.str, ImGuiTreeNodeFlags_DefaultOpen)) {

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
		if (ImGui::IsItemFocused()) //Misleading name, checks if the last item is focused, here it means the WidgetsArea
			Focus();
		else
			Visible = false;
	}

	ImGui::NewLine();

	sidebar::WirelessControl();
	if (ImGui::IsWindowAppearing() || !ImGui::IsAnyItemFocused())
		Focus();

	ImGui::Spacing();
	sidebar::BrightnessControl();
}
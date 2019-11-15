#include "Sidebar.hpp"

#include "../rendering/ImguiExt.hpp"
#include "../../utils.hpp"
#include "SidebarControls.hpp"
#include <string>
#include "../UI.hpp"

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
	
	if (BackPressed()) 
		Visible = false;			
}

bool Sidebar::HighFreqUpdate() 
{			
	for (WinPtr& win : Windows)
		if (win->HighFreqUpdate())
			return true;
	return false;
}

bool Sidebar::ShouldRender()
{
	return Visible;
}
 
inline void Sidebar::PushStyling()
{
	ImGui::Begin("sidebar", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos({1280 - W, 0});
	ImGui::SetWindowSize({Sidebar::W, Sidebar::H});
}

inline void Sidebar::PopStyiling()
{
	ImGui::End();
}

inline void Sidebar::DoUpdate()
{			
	auto s = &console::Status.DateTime;
	ImGui::PushFont(font30);
	ImGui::Text("%d:%d   %d/%d", s->hour, s->minute, s->day, s->month);
	ImGui::SameLine();
	ImGui::TextRight("%d%%", console::Status.BatteryLevel);
	ImGui::PopFont();
	ImGui::Spacing();

	ImGui::BeginChild("WidgetsArea", {W - 15, H * 3 / 4});
	for (WinPtr& win : Windows)
		if (win->ShouldRender())
			win->Update();
	ImGui::EndChild();
	
	ImGui::NewLine();

	sidebar::WirelessControl();
	ImGui::Spacing();
	sidebar::BrightnessControl();
}

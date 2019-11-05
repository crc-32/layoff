#include "Sidebar.hpp"

#include <imgui/imgui.h>
#include "../../utils.hpp"
#include "SidebarControls.hpp"

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
	ImGui::Text("%d:%d %d/%d - %d%%", s->hour, s->minute, s->day, s->month, console::Status.BatteryLevel);
	ImGui::Spacing();

	if (ImGui::CollapsingHeader("System", ImGuiTreeNodeFlags_DefaultOpen))
	{
		sidebar::WirelessControl();
		sidebar::BrightnessControl();
	}
	
	for (WinPtr& win : Windows)
		if (win->ShouldRender())
			win->Update();
}

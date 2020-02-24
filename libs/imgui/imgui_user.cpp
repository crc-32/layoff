#include "imgui_user.h"
#include "imgui_internal.h"

void ImGui::SelectItem(bool enableNav, ImGuiID ID)
{
	auto win = ImGui::GetCurrentWindow();
	if (ID == 0)
	{
		ID = win->DC.LastItemId;
		ImGui::SetScrollHereY();
	}
	ImGui::SetFocusID(ID, win);
	GImGui->NavDisableHighlight = !enableNav;
	GImGui->NavInitResultId = ID;
}
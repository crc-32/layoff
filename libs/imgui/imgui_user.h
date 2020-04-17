#pragma once
#include "imgui.h"

namespace ImGui
{
	void SelectItem(bool enableNav = true, ImGuiID ID = 0);
	void TextRight(const char* fmt, ...);
	void TextRightV(const char* fmt, va_list args);
}
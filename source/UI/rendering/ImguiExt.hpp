#pragma once
#include <imgui/imgui.h>

namespace ImGui 
{
	void TextRight(const char* fmt, ...);
	void TextRightV(const char* fmt, va_list args);
}
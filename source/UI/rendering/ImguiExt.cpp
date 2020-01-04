#include "ImguiExt.hpp"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui_internal.h>

using namespace ImGui;

static void TextRightEx(const char* text, const char* text_end)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	const char* text_begin = text;
	if (text_end == NULL)
		text_end = text + strlen(text); // FIXME-OPT

	const float wrap_pos_x = window->DC.TextWrapPos;
	const bool wrap_enabled = (wrap_pos_x >= 0.0f);

	const float wrap_width = wrap_enabled ? CalcWrapWidthForPos(window->DC.CursorPos, wrap_pos_x) : 0.0f;
	const ImVec2 text_size = CalcTextSize(text_begin, text_end, false, wrap_width);

	const ImVec2 text_pos(window->Pos.x + window->Size.x - text_size.x - window->WindowPadding.x, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);

	ImRect bb(text_pos, text_pos + text_size);
	ItemSize(text_size);
	if (!ItemAdd(bb, 0))
		return;

	// Render (we don't hide text after ## in this end-user function)
	RenderTextWrapped(bb.Min, text_begin, text_end, wrap_width);
}

void ImGui::TextRight(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	TextRightV(fmt, args);
	va_end(args);
}

void ImGui::TextRightV(const char* fmt, va_list args)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImGuiContext& g = *GImGui;
	const char* text_end = g.TempBuffer + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
	TextRightEx(g.TempBuffer, text_end);
}
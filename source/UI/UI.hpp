#pragma once

#include "rendering/deko3d/renderer.h"
#include <imgui/imgui.h>
#include <switch.h>

#define DEFAULT_WIN_WIDTH 1280
#define DEFAULT_WIN_HEIGHT 720

static inline void ImguiInitialize()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT);

	ImGuiStyle& style = ImGui::GetStyle();
	style.AntiAliasedLines = false;
	style.AntiAliasedFill = false;
	ImGui::StyleColorsDark(&style);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f,0.0f,0.0f,0.9f));
}

void UIInit();
void UIExit();
void FrameStart();
void ClearFramebuffer();
void FrameEnd();

namespace layoff::UI {
	void SlowMode();
	void FastMode();
}
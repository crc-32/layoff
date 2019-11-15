#pragma once

#include <imgui/imgui.h>
#include <switch.h>

#define DEFAULT_WIN_WIDTH 1280
#define DEFAULT_WIN_HEIGHT 720

extern ImFont* Font25;
extern ImFont* Font30;

static inline void ImguiInitialize()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT);

	ImGuiStyle& style = ImGui::GetStyle();
	style.AntiAliasedLines = false;
	style.AntiAliasedFill = false;
	style.WindowRounding = 0.0f;
}

void UIInit();
void UIExit();
void FrameStart();
void ClearFramebuffer();
void FrameEnd();
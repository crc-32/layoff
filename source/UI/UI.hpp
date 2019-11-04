#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_freetype.h>
#include "rendering/imgui_sw.hpp"
#include <switch.h>

void ImguiBindInputs(ImGuiIO& io)
{
	hidScanInput();
	u32 touch_count = hidTouchCount();
	if (touch_count == 1)
	{
		touchPosition touch;
		hidTouchRead(&touch, 0);
		io.MousePos = ImVec2(touch.px, touch.py);
		io.MouseDown[0] = true;
	}       
	else io.MouseDown[0] = false;
	
	u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
	
	io.NavInputs[ImGuiNavInput_DpadDown] = kHeld & KEY_DOWN;
	io.NavInputs[ImGuiNavInput_DpadUp] = kHeld & KEY_UP;
	io.NavInputs[ImGuiNavInput_DpadLeft] = kHeld & KEY_LEFT;
	io.NavInputs[ImGuiNavInput_DpadRight] = kHeld & KEY_RIGHT;

	io.NavInputs[ImGuiNavInput_Activate] = kHeld & KEY_A;
	io.NavInputs[ImGuiNavInput_Cancel] = kHeld & KEY_B;
	io.NavInputs[ImGuiNavInput_Menu] = kHeld & KEY_X;
	io.NavInputs[ImGuiNavInput_FocusNext] = kHeld & (KEY_ZR | KEY_R);
	io.NavInputs[ImGuiNavInput_FocusPrev] = kHeld & (KEY_ZL | KEY_L);
}

Framebuffer fb;

#define DEFAULT_WIN_WIDTH 1280
#define DEFAULT_WIN_HEIGHT 720

void UIInit(NWindow *win) {
    Result rc = framebufferCreate(&fb, win, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT, PIXEL_FORMAT_RGBA_8888, 2);
	if (R_FAILED(rc))
		fatalSimple(rc);
    
    rc = framebufferMakeLinear(&fb);
	if (R_FAILED(rc))
		fatalSimple(rc);

	/* ==ImGui init== */
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.DisplaySize = ImVec2(DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT);

	/* ==Font init== */
	rc = plInitialize();
	IM_ASSERT("plInitialize failed" && R_SUCCEEDED(rc));

	PlFontData font;
    rc = plGetSharedFontByType(&font, PlSharedFontType_Standard);
	IM_ASSERT("plGetSharedFontByType failed" && R_SUCCEEDED(rc));

	io.Fonts->AddFontFromMemoryTTF((void*)font.address, font.size, 25.0f);
	ImGuiFreeType::BuildFontAtlas(io.Fonts, 0u);

	imgui_sw::bind_imgui_painting();
}

void UIStart() {
	ImGui::NewFrame();
}

void UIUpdate() {
	ImGui::Render();
	ImGuiIO &io = ImGui::GetIO();
	ImguiBindInputs(io);
	u32 *pixels = (u32*)framebufferBegin(&fb, nullptr);
	memset(pixels, 0, sizeof(u32) * DEFAULT_WIN_WIDTH * DEFAULT_WIN_HEIGHT);
	imgui_sw::paint_imgui(pixels, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT);
	framebufferEnd(&fb);
}
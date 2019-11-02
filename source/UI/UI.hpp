#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_sw.hpp>
#include <imgui/imgui_freetype.h>
#include <switch.h>

// Uses layer dimensions logic (docked screen size reference even when in handheld mode)
#define VERT_LAYER_SIZE 1080
#define HOR_LAYER_SIZE 650
// True FB resolution
#define VERT_RES VERT_LAYER_SIZE/2
#define HOR_RES HOR_LAYER_SIZE/2

Framebuffer fb;
NWindow *win;
Event vsync;

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

void UIInit(NWindow *window) {
    /* ==Switch FB init== */
    win = window;
    if(!win)
	{
		fatalSimple(MAKERESULT(255,120));
	}

    Result rc = framebufferCreate(&fb, win, HOR_RES, VERT_RES, PIXEL_FORMAT_RGBA_8888, 2);
	if (R_FAILED(rc))
		fatalSimple(rc);
    
    rc = framebufferMakeLinear(&fb);
	if (R_FAILED(rc))
		fatalSimple(rc);

	ViDisplay disp;
	rc = viGetDisplayVsyncEvent(&disp, &vsync);

	/* ==ImGui init== */
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.DisplaySize = ImVec2(HOR_RES, VERT_RES);

	/* ==Font init== */
	rc = plInitialize();
	IM_ASSERT("plInitialize failed" && R_SUCCEEDED(rc));

	PlFontData font;
    rc = plGetSharedFontByType(&font, PlSharedFontType_Standard);
	IM_ASSERT("plGetSharedFontByType failed" && R_SUCCEEDED(rc));

	io.Fonts->AddFontFromMemoryTTF((void*)font.address, font.size, 25.0f);
	unsigned int flags = 0;
	ImGuiFreeType::BuildFontAtlas(io.Fonts, flags);

	imgui_sw::bind_imgui_painting();
}

void UIStart() {
	ImGui::NewFrame();
}

void UIUpdate() {
	ImGui::Render();
	ImGuiIO &io = ImGui::GetIO();
	eventWait(&vsync, 1e+6);
	ImguiBindInputs(io);
	u32 stride;
	u32 *pixels = (u32*)framebufferBegin(&fb, &stride);
	memset(pixels, 0, sizeof(u32)*HOR_RES*VERT_RES);
	imgui_sw::paint_imgui(pixels, HOR_RES, VERT_RES);
	framebufferEnd(&fb);
}
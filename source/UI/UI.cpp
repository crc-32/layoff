#include "UI.hpp"
#include <switch.h>

Framebuffer fb;

ImFont* Font25;
ImFont* Font30;

#define DEFAULT_WIN_WIDTH 1280
#define DEFAULT_WIN_HEIGHT 720

void UIInit() {
	Result rc = plInitialize();
	if (R_FAILED(rc))
		fatalThrow(rc);

	ImguiInitialize();
	renderer::init();
	plExit();
}

void FrameStart() {
	renderer::newFrame();
	ImGui::NewFrame();
}

void ClearFramebuffer()
{
		
}

void FrameEnd() {	
	ImGui::Render();
	renderer::render();
}
#include "UI.hpp"
#include <switch.h>

Framebuffer fb;

void UIInit() {
	Result rc = plInitialize();
	if (R_FAILED(rc))
		fatalThrow(rc);

	ImguiInitialize();
	renderer::init();
}

void FrameStart() {
	renderer::newFrame();
}

void FrameEnd() {	
	renderer::render();
}

void layoff::UI::SlowMode() {
	renderer::SlowMode();
}

void layoff::UI::FastMode() {
	renderer::FastMode();
}
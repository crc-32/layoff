#include "UI.hpp"
#include <switch.h>

void UIInit(NWindow *win) {
	Result rc = plInitialize();
	if (R_FAILED(rc))
		fatalThrow(rc);

	ImguiInitialize();
	renderer::init(win);
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
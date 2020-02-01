//Functions related to the general state of layoff
#pragma once
#include "NotificationManager.hpp"

#include <switch.h>
#include "UI/Window.hpp"

extern u64 kDown;
extern u64 kHeld;
static inline bool BackPressed() { return kDown & KEY_B; }

namespace layoff {

	enum class OverlayMode
	{
		Active, //Layoff has the focus and the current app won't get inputs
		Idle, //Layoff is in idle mode and won't draw anything nor get inputs
		Passive, //A window has the focus but the current app will still receive inputs
	};
	
	bool IsHomeLongPressed();
	bool IsHomePressed();
	bool IsPowerPressed();
	
	OverlayMode GetCurrentMode();

	void SwitchToActiveMode();
	void SwitchToPassiveMode();

	extern NotificationManager *nman;
}

#if LAYOFF_LOGGING
	#include <string>
	void PrintLn(const std::string& s);
	void Print(const std::string& s);	
#else 
	#define PrintLn(x) do {} while(0)
	#define Print(x) do {} while(0)
#endif
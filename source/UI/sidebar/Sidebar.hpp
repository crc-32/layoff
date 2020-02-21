
// The sidebar is layoff's main window, it also hosts most of the other windows

#pragma once

#include <switch.h>
#include "../Window.hpp"
#include <vector>

namespace layoff::UI {
	
	class Sidebar : public Window
	{	
	public: 
		static constexpr u32 W = 512;
		static constexpr u32 H = 720; 
	
		bool Visible = true;
	
		void RequestClose() override;
		void Update() override;
		bool HighFreqUpdate() override;		
		bool ShouldRender() override;
		
		void FocusSidebar();

		~Sidebar() override {	}
	private: 
		void PushStyling();		
		void PopStyiling();
		
		void DoUpdate();
	};

}
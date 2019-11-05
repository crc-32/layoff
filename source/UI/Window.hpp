#pragma once

#include <memory>

namespace layoff::UI {

	class Window
	{
	public:
		virtual void Update() = 0;
		virtual void RequestClose() {}
		
		/*
			Not sure about this one, the idea is that normally the active loop would refresh at
			2 fps unless the user interacts and this property is for plugins that need to override that.
			But there's the problem of imgui requiring a few cycles to properly render everything so it
			would need extra logic to count a few cycles anyway after the user interaction, will think 
			if it's worth keeping this, in the meantime it's not used by anything. 
		*/
		virtual bool HighFreqUpdate() { return false; }
		virtual bool ShouldRender() { return true;} 
		
		virtual ~Window() {}
	};
	
	using WinPtr = std::unique_ptr<Window>;
}
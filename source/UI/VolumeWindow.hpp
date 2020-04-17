#include "Window.hpp"
#include <switch.h>
#include <atomic>
#include <imgui/imgui.h>

namespace layoff::UI
{
	class VolumeWindow : public Window 
	{
	public:
		static const s64 MaxOnScreenTime;
		static const ImVec2 Size;

		void RequestClose() override { }

		void Update() override;
		bool ShouldRender() override;
	
		void Signal(u8 volume);
	private:
		std::atomic<s64> ShowTime = 0;
		std::atomic<u8> Volume = 0;
	};
}
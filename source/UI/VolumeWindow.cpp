#include "VolumeWindow.hpp"
#include <time.h>

namespace layoff::UI
{
	void VolumeWindow::Update()
	{
		ImGui::Begin("volume", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
		ImGui::SetWindowPos({ 0,0 });
		ImGui::SetWindowSize(Size);
		//Size.x - 60 is the total length of the volume bar
		ImGui::GetWindowDrawList()->AddRectFilled({ 30, Size.y / 2 - 4 }, { Size.x - 30 , Size.y / 2 + 4 }, 0xFF5A5A5A);
		ImGui::GetWindowDrawList()->AddRectFilled({ 30, Size.y / 2 - 4 }, { 30 + (Size.x - 60) * Volume / 15.0 , Size.y / 2 + 4 }, 0xFFFFFFFF);
		ImGui::End();
	}
	
	bool VolumeWindow::ShouldRender()
	{
		return ShowTime + MaxOnScreenTime > time(NULL);
	}

	void VolumeWindow::Signal(u8 volume)
	{
		Volume = volume;
		ShowTime = time(NULL);
	}

	const ImVec2 VolumeWindow::Size = { 280, 75 };
	const s64 VolumeWindow::MaxOnScreenTime = 5;
}
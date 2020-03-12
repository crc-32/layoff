#pragma once
#if LAYOFF_LOGGING

#include <imgui/imgui.h>
#include "../utils.hpp"
#include "../UI/Window.hpp"
#include <string>

#include "imgui_memory_editor.h"

#include "../UI/images/Image.hpp"

namespace layoff::debug {
	
	class DebugWindows : public layoff::UI::Window
	{	
	public: 
		DebugWindows()
		{
			mutexInit(&mutex);
			trollface = layoff::UI::images::FromFile("romfs:/trollface.png");
		}

		bool Visible = true;
		void RequestClose() override 
		{
			Visible = false;
		}
	
		void Update() override 
		{
			layoff::IPC::ScopeLock lock(mutex);

			ImGui::Begin("Log window", &Visible, ImVec2(300, 400));
			
			if (trollface->GetBuffer())
				ImGui::Image(trollface->GetID(), trollface->GetSize());
			
			ImGui::Text(LogText.c_str());
			ImGui::End();

			if (HexBuf.size() != 0)
				mem_edit_1.DrawWindow("Hex editor", HexBuf.data(), HexBuf.size(), 0x0000);
		}
		
		bool ShouldRender() override
		{
			return Visible;
		}

		void PrintLine(const std::string& text)
		{
			layoff::IPC::ScopeLock lock(mutex);
			LogText += text + "\n";
		}

		void Print(const std::string& text)
		{
			layoff::IPC::ScopeLock lock(mutex);
			LogText += text;
		}

		void SetHexBuf(const u8* data, u32 len)
		{
			layoff::IPC::ScopeLock lock(mutex);
			if (data)
				HexBuf = std::vector<u8>(data, data + len);
			else
				HexBuf.clear();
		}
		
		~DebugWindows() override
		{

		}
	private:
		std::string LogText = "";
		std::vector<u8> HexBuf;
		MemoryEditor mem_edit_1;
		Mutex mutex;

		layoff::UI::ImagePtr trollface = nullptr;
	};

	static DebugWindows Instance;
}

void PrintLn(const std::string& s)
{
	layoff::debug::Instance.PrintLine(s);
}

void Print(const std::string& s)
{
	layoff::debug::Instance.Print(s);
}

void PrintHex(const u8* data, u32 len)
{
	layoff::debug::Instance.SetHexBuf(data, len);
}
#endif
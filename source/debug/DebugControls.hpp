#pragma once
#if LAYOFF_LOGGING

#include <imgui/imgui.h>
#include "../utils.hpp"
#include "../UI/Window.hpp"
#include <string>

#include "imgui_memory_editor.h"

namespace layoff::debug {
	
	class DebugWindows : public layoff::UI::Window
	{	
	public: 
		DebugWindows()
		{
			mutexInit(&mutex);
		}

		bool Visible = true;
		void RequestClose() override 
		{
			Visible = false;
		}
	
		void Update() override 
		{
			ScopeLock lock(this);

			ImGui::Begin("Log window", &Visible, ImVec2(300, 400));
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
			ScopeLock lock(this);
			LogText += text + "\n";
		}

		void Print(const std::string& text)
		{
			ScopeLock lock(this);
			LogText += text;
		}

		void SetHexBuf(const u8* data, u32 len)
		{
			ScopeLock lock(this);
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

		class ScopeLock 
		{
		public:
			ScopeLock(DebugWindows* t) : ref(t) { mutexLock(&ref->mutex); }
			~ScopeLock() { mutexUnlock(&ref->mutex); }
		private:
			DebugWindows* ref;
		};

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
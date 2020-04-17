#pragma once
#include "IPCControl.hpp"
#include <imgui/imgui.h>
#include <vector>

namespace layoff::UI::IPC {

	class TextPanel : public Control
	{
	public:
		TextPanel(const char* str, int len, LayoffIdentifier id) : string(str, str + len)
		{
			ID = id;
			string[string.size() - 1] = '\0';
		}

		void Update() override
		{
			ImGui::TextWrapped("%s", string.data());
		}

		bool SignalEvent() const override
		{
			return false;
		}

		LayoffUIEvent GetEvent()
		{
			return {0};
		}

		~TextPanel() override { }
	private:
		std::vector<char> string;
	};

	using ControlPtr = std::unique_ptr<Control>;
}
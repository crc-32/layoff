#pragma once
#include "IPCControl.hpp"
#include <imgui/imgui.h>
#include <vector>
#include "../sidebar/Sidebar.hpp"

#include <imgui/imgui_internal.h>

namespace layoff::UI::IPC {

	class ButtonList : public Control
	{
	public:
		ButtonList(const LayoffName* data, u8 count, LayoffIdentifier id) : names(data, data + count)
		{
			ID = id;
			for (auto& n : names)
				n.str[sizeof(n.str) - 1] = 0;
		}

		void Update() override
		{
			int i = 0;
			for (auto& n : names)
			{
				if (ImGui::Button(n.str, { layoff::UI::Sidebar::W - 10, 0 }))
				{
					lastPress = i;
					ImGui::FocusWindow(ImGui::GetCurrentWindow());
				}
				i++;
			}
		}

		bool SignalEvent() const override
		{
			return lastPress != -1;;
		}

		LayoffUIEvent GetEvent()
		{
			if (SignalEvent())
			{
				LayoffUIEvent val = { ID };
				val.data1 = lastPress;
				lastPress = -1;
				return val;
			}
			else
				return { 0 };
		}

		~ButtonList() override { }
	private:
		std::vector<LayoffName> names;
		int lastPress = -1;
	};

	using ControlPtr = std::unique_ptr<Control>;
}
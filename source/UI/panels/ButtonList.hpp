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

		virtual void Update() override
		{
			int i = 0;
			for (auto& n : names)
			{
				if (ImGui::Button(n.str, { layoff::UI::Sidebar::W - 10, 0 }))
				{
					eventData = i;
					ImGui::FocusWindow(ImGui::GetCurrentWindow());
				}
				i++;
			}
		}

		bool SignalEvent() const override
		{
			return eventData != UINT64_MAX;
		}

		LayoffUIEvent GetEvent()
		{
			if (SignalEvent())
			{
				LayoffUIEvent val = { };
				val.panel = ID;
				val.data1 = eventData;
				val.data2 = 0;
				eventData = UINT64_MAX;
				return val;
			}
			else
				return { 0 };
		}

		~ButtonList() override { }
	protected:
		std::vector<LayoffName> names;
		u64 eventData = UINT64_MAX;
	};
}
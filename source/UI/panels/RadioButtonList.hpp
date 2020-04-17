#pragma once
#include "ButtonList.hpp"

namespace layoff::UI::IPC {

	class RadioButtonList : public ButtonList
	{
	public:
		RadioButtonList(const LayoffName* data, u8 count, LayoffIdentifier id) : ButtonList(data, count, id) {}

		void Update() override
		{
			int i = 0;
			for (auto& n : names)
			{
				if (ImGui::RadioButton(n.str, active == i))
				{
					active = i;
					eventData = i;
					ImGui::FocusWindow(ImGui::GetCurrentWindow());
				}
				i++;
			}
		}

		~RadioButtonList() override { }
	private:
		int active = -1;
	};
}
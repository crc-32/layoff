#pragma once
#include "ButtonList.hpp"

namespace layoff::UI::IPC {

	class CheckBoxList : public ButtonsListBase
	{
	public:
		CheckBoxList(const LayoffName* data, u8 count, LayoffIdentifier id, u64 inlineFlags) : ButtonsListBase(data, count, id, inlineFlags) {}

		void Update() override
		{
			int i = 0;
			for (auto& n : names)
			{
				if (inlineFlags & (1ull << i))
					ImGui::SameLine();

				bool curChecked = IsChecked(i);
				if (ImGui::Checkbox(n.str, &curChecked))
				{
					SetChecked(i, curChecked);
					eventData = Checked;
				}
				i++;
			}
		}

		~CheckBoxList() override { }
	private:
		u64 Checked = 0;

		inline bool IsChecked(int index)
		{
			return (Checked >> index) & 1;
		}

		inline void SetChecked(int index, bool value)
		{
			if (value)
				Checked |= 1 << index;
			else
				Checked &= ~(1 << index);
		}
	};
}
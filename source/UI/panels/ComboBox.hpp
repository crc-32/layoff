#pragma once
#include "ButtonList.hpp"

namespace layoff::UI::IPC {

	class ComboBox : public ButtonList
	{
	public:
		ComboBox(const LayoffName* label, const LayoffName* data, u8 count, LayoffIdentifier id) : ButtonList(data, count, id, 0)
		{
			this->label = *label;
		}

		void Update() override
		{
			//Not implemented atm
			//if (inlineFlags)
			//	ImGui::SameLine();

			if (ImGui::Combo(label.str, &selected, items_getter, &names, names.size(), -1))
				eventData = selected;
		}

		~ComboBox() override { }
	private:

		static inline bool items_getter(void* data, int idx, const char** out_text)
		{
			if (!data) return false;
			std::vector<LayoffName>& names = *(std::vector<LayoffName>*)data;
			if (idx >= names.size()) return false;
			*out_text = names[idx].str;
			return true;
		}

		int selected = 0;
		LayoffName label;
	};
}
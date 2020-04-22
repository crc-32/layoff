#pragma once
#include "ButtonList.hpp"

namespace layoff::UI::IPC {

	class ComboBox : public ButtonsListBase
	{
	public:
		ComboBox(const LayoffName* label, const LayoffName* data, u8 count, LayoffIdentifier id) : ButtonsListBase(data, count, id, 0)
		{
			this->label = *label;
		}

		void Update() override
		{
			if (ImGui::Combo(label.str, &selected, items_getter, &names, names.size(), -1))
				eventData = selected;
		}

		~ComboBox() override { }
	private:

		static inline bool items_getter(void* data, int idx, const char** out_text)
		{
			if (!data || idx < 0) return false;
			std::vector<LayoffName>& names = *(std::vector<LayoffName>*)data;
			if ((size_t)idx >= names.size()) return false;
			*out_text = names[idx].str;
			return true;
		}

		int selected = 0;
		LayoffName label;
	};
}
#include "IPCControl.hpp"
#include "TextPanel.hpp"
#include "ButtonList.hpp"
#include "CheckBoxList.hpp"
#include "ComboBox.hpp"
#include "RadioButtonList.hpp"
#include <switch.h>
#include "../../IPC/IPCLock.hpp"
#include "../../IPC/ErrorCodes.h"
#include "../../utils.hpp"

namespace layoff::UI::IPC {
	ControlPtr ParseControl(const LayoffUIHeader& header, const u8* data, const u32 len)
	{
		//TODO consider text encoding support

		if (header.panelID <= 0) return nullptr; //ID 0 is invalid as it's used to signal no event
		
		switch (header.kind)
		{
			case LayoffUIKind_TextBlock:
				return std::make_unique<TextPanel>((const char*)data, len, header.panelID);
			case LayoffUIKind_ButtonList:
			{
				const LayoffUIButtonList* lst = (const LayoffUIButtonList*)data;
				return std::make_unique<ButtonList>(lst->data, lst->count, header.panelID);
			}
			case LayoffUIKind_CheckBoxList:
			{
				const LayoffUICheckBoxList* lst = (const LayoffUICheckBoxList*)data;
				return std::make_unique<CheckBoxList>(lst->data, lst->count, header.panelID);
			}
			case LayoffUIKind_RadioButtonList:
			{
				const LayoffUIRadioButtonList* lst = (const LayoffUIRadioButtonList*)data;
				return std::make_unique<RadioButtonList>(lst->data, lst->count, header.panelID);
			}
			case LayoffUIKind_ComboBox:
			{
				const LayoffUIComboBox* lst = (const LayoffUIComboBox*)data;
				return std::make_unique<ComboBox>(&lst->label, lst->entries.data, lst->entries.count, header.panelID);
			}
			default:
			{
				if (header.kind != LayoffUIKind_None)
					PrintLn("Unknown UI panel type: " + std::to_string(header.kind));
				return nullptr;
			}
		}
	}
}
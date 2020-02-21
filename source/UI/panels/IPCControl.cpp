#include "IPCControl.hpp"
#include "TextPanel.hpp"
#include "ButtonList.hpp"
#include <switch.h>
#include "../../IPC/IPCLock.hpp"
#include "../../IPC/ErrorCodes.h"

namespace layoff::UI::IPC {
	ControlPtr ParseControl(const LayoffUIHeader& header, const u8* data, const u32 len, Result* rc)
	{
		//TODO consider text encoding support
		
		switch (header.kind)
		{
			case LayoffUIKind_TextBlock:
				return std::make_unique<TextPanel>((const char*)data, len, header.panelID);
			case LayoffUIKind_ButtonList:
			{
				const LayoffUIButtonList *lst = (const LayoffUIButtonList*)data;
				return std::make_unique<ButtonList>(lst->data, lst->ButtonCount, header.panelID);
			}
			default:
				if (header.kind != LayoffUIKind_None)
					*rc = ERR_UNKNOWN_UI_TYPE;
				return nullptr;
		}
	}


}
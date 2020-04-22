#pragma once
#include "IPCControl.hpp"
#include <imgui/imgui.h>
#include <vector>
#include "../sidebar/Sidebar.hpp"

#include <imgui/imgui_internal.h>

#include "../../utils.hpp"

namespace layoff::UI::IPC {

	class ButtonsListBase : public Control 
	{
	public:
		ButtonsListBase(const LayoffName* data, u8 count, LayoffIdentifier id, u64 inlineFlags)
		{
			ID = id;
			this->inlineFlags = inlineFlags;

			if (count > 62)
			{
				PrintLn("ButtonList::ctor() count is " + std::to_string(count));
				count = 62;
			}

			names = std::vector<LayoffName>(data, data + count);
			for (auto& n : names)
				n.str[sizeof(n.str) - 1] = 0;
		}

		bool SignalEvent() const override
		{
			return eventData != UINT64_MAX;
		}

		LayoffUIEvent GetEvent() override
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

	protected:
		std::vector<LayoffName> names;
		u64 eventData = UINT64_MAX;
		u64 inlineFlags = 0;
	};

	class ButtonList : public ButtonsListBase
	{
	public:
		ButtonList(const LayoffName* data, u8 count, LayoffIdentifier id, u64 inlineFlags) : ButtonsListBase(data, count, id, inlineFlags)
		{
			CalculateElemsPerLine();
		}

		virtual void Update() override
		{
			int i = 0;

			int* curLine = lineCount.data();
			int lineElems = 0;

			for (auto& n : names)
			{
				if (++lineElems < *curLine)
				{
					ImGui::SameLine();
				}
				else
				{
					curLine++;
					lineElems = 0;
				}

				float width = 0;

				if (curLine == lineCount.data()) //First line, the first button may be next to some other component
					width = 0; //Auto
				else if (*curLine == 0)
					width = layoff::UI::Sidebar::W - 10;
				else
					width = (layoff::UI::Sidebar::W - 10) / (float)*curLine - 2;

				if (ImGui::Button(n.str, { width, 0 }))
				{
					eventData = i;
					ImGui::FocusWindow(ImGui::GetCurrentWindow());
				}
				i++;
			}
		}		

	protected:
		std::vector<int> lineCount;

		void CalculateElemsPerLine()
		{
			int CurLineCount = 1;
			for (size_t i = 0; i < names.size(); i++)
			{
				if (inlineFlags & (1llu << i))
					CurLineCount++;
				else
				{
					lineCount.push_back(CurLineCount);
					CurLineCount = 1;
				}
			}
			
			lineCount.push_back(CurLineCount);
		}
	};
}
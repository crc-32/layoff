#pragma once
#include <string>
#include <sstream>
#include <limits>

extern "C"{
#include "../dmntcht/dmntcht.h"
}

class CheatScreen : public UiItem
{
public:
	CheatScreen() 
	{
		dmntchtInitialize();
		dmntchtForceOpenCheatProcess();
		
		MemoryInfo meminfo = { 0 };
		u64 lastAddr = 0;
	
		do {
			lastAddr = meminfo.addr;
			dmntchtQueryCheatProcessMemory(&meminfo, meminfo.addr + meminfo.size);

			memSections.push_back(meminfo);
		} while (lastAddr < meminfo.addr + meminfo.size);
	}
	
	~CheatScreen()
	{
		dmntchtExit();
	}

	bool Draw()
	{
		bool Run = true;
		if(renderDirty > 0)
		{
		if (!ImGui::Begin("Ram editor", &Run))
		{
			ImGui::End();
			return Run;
		}
		if (DisplayDialog != "")
			DrawDialog();
		else
			DrawMainMenu();
		
		ImGui::End();
		}
		return Run;
	}
private:
	string DisplayDialog = "";
	void DrawDialog() 
	{
		ImGui::Text("%s", DisplayDialog.c_str());
		if (ImGui::Button("  OK  "))
			DisplayDialog = "";
	}

	void DrawMainMenu() {
		if (ImGui::CollapsingHeader("Value search"))
		{
			ImGui::Text("Value type:");
#define typeSelect(typeName, typeVal) if (ImGui::RadioButton(typeName, searchType == ValueType::typeVal)) {searchType = ValueType::typeVal;}
			typeSelect("byte", U8); ImGui::SameLine();
			typeSelect("char", S8); ImGui::SameLine();
			typeSelect("ushort", U16); ImGui::SameLine();
			typeSelect("short", S16); ImGui::SameLine();
			typeSelect("uint", U32);
			typeSelect("int", S32); ImGui::SameLine();
			typeSelect("ulong", U64); ImGui::SameLine();
			typeSelect("long", S64); ImGui::SameLine();
			typeSelect("double", Double); ImGui::SameLine();
			typeSelect("float", Float);
#undef typeSelect
			ImGui::Text("%s", DisplayValue.c_str());
#define DIGIT(x) DisplayValue += x;
			if (ImGui::Button(" 0 ")) { DIGIT("0") } ImGui::SameLine();
			if (ImGui::Button(" 1 ")) { DIGIT("1") } ImGui::SameLine();
			if (ImGui::Button(" 2 ")) { DIGIT("2") } ImGui::SameLine();
			if (ImGui::Button(" 3 ")) { DIGIT("3") } ImGui::SameLine();
			if (ImGui::Button(" 4 ")) { DIGIT("4") }
			if (ImGui::Button(" 5 ")) { DIGIT("5") } ImGui::SameLine();
			if (ImGui::Button(" 6 ")) { DIGIT("6") } ImGui::SameLine();
			if (ImGui::Button(" 7 ")) { DIGIT("7") } ImGui::SameLine();
			if (ImGui::Button(" 8 ")) { DIGIT("8") } ImGui::SameLine();
			if (ImGui::Button(" 9 ")) { DIGIT("9") }
			if (ImGui::Button(" . ")) { DIGIT(".") } ImGui::SameLine();
			if (ImGui::Button(" - "))
			{
				if (DisplayValue.size() != 0 && DisplayValue[0] == '-')
					DisplayValue = DisplayValue.substr(1, DisplayValue.size() - 1);
				else DisplayValue = "-" + DisplayValue;
			}
			ImGui::SameLine();
			if (ImGui::Button(" C ")) { DisplayValue = DisplayValue.substr(0, DisplayValue.size() - 1); } ImGui::SameLine();
			if (ImGui::Button(" CE ")) { DisplayValue = ""; }
			if (ImGui::Button(" Search ")) { Search(); } ImGui::SameLine();
			if (ImGui::Button(" Clear results ")) { ClearResults(); }
#undef DIGIT
		}
		if (ImGui::CollapsingHeader("Search results"))
		{
			if (Results.size() == 0)
				ImGui::Text("No results yet");
			else if (Results.size() <= 100)
			{
				ImGui::ListBox("##SRes", &ResultsIndex, &StrVecGetter, &vResults, vResults.size(), -1);
				if (ImGui::Button("Add to offsets"))
				{
					Offsets.push_back(Results[ResultsIndex]);
					UpdateView();
				}
			}
			else ImGui::Text("Too many results !");
		}
		if (ImGui::CollapsingHeader("Offsets"))
		{
			if (Offsets.size() == 0)
				ImGui::Text("No results yet");
			else
			{
				ImGui::ListBox("##Offs", &OffsIndex, &StrVecGetter, &vOffsets, vOffsets.size(), -1);
				if (ImGui::Button("Set value")) SetValue();
				ImGui::SameLine();
				if (ImGui::Button("Remove"))
				{
					Offsets.erase(Offsets.begin() + OffsIndex);
					vOffsets.erase(vOffsets.begin() + OffsIndex);
					OffsIndex--;
					if (OffsIndex < 0) OffsIndex = 0;
				} 
				ImGui::SameLine();
				if (ImGui::Button("Remove all")) {
					Offsets.clear();
					OffsIndex= 0;
				} 
				ImGui::SameLine();
			}
		}
	}

	string DisplayValue = "";

	struct RamAddr {
		u64 addr;
		MemoryType type;
	};

	enum class ValueType : int
	{
		U8 = 0, U16, U32, U64, S8, S16, S32, S64, Float, Double, TYPE_MAX
	};
	ValueType searchType = ValueType::S32;

	vector<MemoryInfo> memSections;
	vector<RamAddr> Results;
	vector<RamAddr> Offsets;

	vector<string> vResults;
	int ResultsIndex = 0;
	vector<string> vOffsets;
	int OffsIndex = 0;

	static bool StrVecGetter(void* data, int n, const char** out_text)
	{
		const vector<string>* v = (vector<string>*)data;
		*out_text = (*v)[n].c_str();
		return true;
	}

	void UpdateView() 
	{
		vResults.clear();
		if (Results.size() <= 100)
			for (auto val : Results)
			{
				stringstream str;
				str << std::uppercase << std::hex << val.addr << endl;
				vResults.push_back(str.str());
			}
		vOffsets.clear();
		for (auto val : Offsets)
		{
			stringstream str;
			str << std::uppercase << std::hex << val.addr << endl;
			vOffsets.push_back(str.str());
		}
	}
	
	void ClearResults() 
	{
		vResults.clear();
		Results.clear();
		ResultsIndex = 0;
	}
	
	void SetValue() 
	{
		if (Offsets.size() <= OffsIndex)
		{
			OffsIndex = 0;
			return;
		}
		auto val = StrValToMem();
		if (val.size <= 0) return;
		dmntchtWriteCheatProcessMemory(Offsets[OffsIndex].addr, &val.value, val.size);
		DisplayDialog = "Done !";
	}

	struct ParsedValue { u64 value; int size; };
	void Search() 
	{
		auto val = StrValToMem();
		if (val.size <= 0) return;
		
		if (Results.size() == 0) //new search
		{
			for (const MemoryInfo &meminfo : memSections) {
				if (meminfo.type != MemType_Heap) continue;
				u64 offset = 0;
				u64 bufferSize = 0x10000;
				u8 *buffer = new u8[bufferSize];
				while (offset < meminfo.size)
				{
					if (meminfo.size - offset < bufferSize)
						bufferSize = meminfo.size - offset;
					
					dmntchtReadCheatProcessMemory(meminfo.addr + offset, buffer, bufferSize);
		
					u64 realValue = 0;
					for (u64 i = 0; i < bufferSize; i++) {
						memcpy(&realValue, buffer + i, val.size);
		
						if (realValue == val.value) 
							Results.push_back({ .addr = meminfo.addr + offset + i, .type = (MemoryType) meminfo.type });
					}
		
					offset += bufferSize;
				}	
				delete[] buffer;
			}
		}
		else //Search in results
		{
			vector<RamAddr> newAddresses;
			for (auto &addr : Results) {
				u64 value = 0;

				dmntchtReadCheatProcessMemory(addr.addr, &value, val.size);

				if (value == val.value) 
					newAddresses.push_back(addr);
				
			}

			Results = std::move(newAddresses);
		}
		UpdateView();
	}

	template <typename D, typename S>
	ParsedValue SizeCheckAndCopyMem(S source)
	{
		ParsedValue res = { 0 };
		if (source > std::numeric_limits<D>::max() || source < std::numeric_limits<D>::min())
		{
			DisplayDialog = "Value outside of type bounds";
			res.size = -1;
			return res;
		}
		D val = static_cast<D>(source);
		res.size = sizeof(val);
		static_assert(sizeof(D) <= sizeof(u64), "Unexpected destination value size");
		memcpy(&res.value, &val, res.size);
		return res;
	}

	ParsedValue StrValToMem()
	{
		try {
			if (searchType <= ValueType::U64)
			{
				u64 value = stoull(DisplayValue);
				switch (searchType)
				{
					case ValueType::U8: return SizeCheckAndCopyMem<u8, u64>(value);
					case ValueType::U16: return SizeCheckAndCopyMem<u16, u64>(value);
					case ValueType::U32: return SizeCheckAndCopyMem<u32, u64>(value);
					case ValueType::U64: return SizeCheckAndCopyMem<u64, u64>(value);
					default: return { 0 ,-1 }; // Silence warnings
				}
			}
			else if (searchType == ValueType::Float || searchType == ValueType::Double)
			{
				double value = stod(DisplayValue);
				switch (searchType)
				{
					case ValueType::Float: return SizeCheckAndCopyMem<float, double>(value);
					case ValueType::Double: return SizeCheckAndCopyMem<double, double>(value);
					default: return { 0 ,-1 };
				}
			}
			else 
			{
				s64 value = stoll(DisplayValue);
				switch (searchType)
				{
					case ValueType::S8: return SizeCheckAndCopyMem<s8, s64>(value);
					case ValueType::S16: return SizeCheckAndCopyMem<s16, s64>(value);
					case ValueType::S32: return SizeCheckAndCopyMem<s32, s64>(value);
					case ValueType::S64: return SizeCheckAndCopyMem<s64, s64>(value);
					default: return { 0 ,-1 };
				}
			}
		}
		catch (...)
		{
			DisplayDialog = "Exception while converting the value";
			DisplayValue = "";
			return { 0 ,-1 };
		}
		return { 0 ,-1 };
	}
};
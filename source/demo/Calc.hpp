#pragma once
#include <iostream>
#include <math.h>
#include "../UI/UI.hpp"


class DemoCalc : public UiItem
{
public:
	bool Draw() override
	{		
		if(renderDirty > 0)
		{
		if (!ImGui::Begin("Calc", &Running) || !Running)
		{
			ImGui::End();
			return Running;
		}
		ImGui::Text("%.1f", DisplayValue);
#define DIGIT(x) DisplayValue += x * pow(10, DispDigits++);
		if (ImGui::Button(" 1 ")) { DIGIT(1) } ImGui::SameLine();
		if (ImGui::Button(" 2 ")) { DIGIT(2) } ImGui::SameLine();
		if (ImGui::Button(" 3 ")) { DIGIT(3) } ImGui::SameLine();
		if (ImGui::Button(" + ")) { Equals(); Op = Sum; DisplayValue = 0; DispDigits = 0; }
		if (ImGui::Button(" 4 ")) {DIGIT(4)} ImGui::SameLine();
		if (ImGui::Button(" 5 ")) {DIGIT(5)} ImGui::SameLine();
		if (ImGui::Button(" 6 ")) {DIGIT(6)} ImGui::SameLine();
		if (ImGui::Button(" x ")) { Equals(); Op = Mult; DisplayValue = 0; DispDigits = 0; }
		if (ImGui::Button(" 7 ")) {DIGIT(7)} ImGui::SameLine();
		if (ImGui::Button(" 8 ")) {DIGIT(8)} ImGui::SameLine();
		if (ImGui::Button(" 9 ")) {DIGIT(9)} ImGui::SameLine();
		if (ImGui::Button(" - ")) { Equals(); Op = Sub; DisplayValue = 0; DispDigits = 0; }
		if (ImGui::Button(" 0 ")) { DisplayValue *= 10; } ImGui::SameLine();
		if (ImGui::Button(" / ")) { Equals(); Op = Div; DisplayValue = 0; DispDigits = 0; } ImGui::SameLine();
		if (ImGui::Button(" = ")) { Equals(); DisplayValue = Tot;  }
#undef DIGIT
		ImGui::End();
		}
		return true;
	}
private:
	bool Running = true;
	float DisplayValue = 0;
	int DispDigits = 0;
	float Tot = 0;

	void Equals() 
	{
		if (Op == OP::Sum)
			Tot += DisplayValue;
		if (Op == OP::Sub)
			Tot -= DisplayValue;
		if (Op == OP::Div)
			Tot /= DisplayValue;
		if (Op == OP::Mult)
			Tot *= DisplayValue;
		Op = NUL;
	}

	enum OP : int
	{
		Sum,
		Div,
		Mult,
		Sub,
		NUL
	};

	OP Op = Sum;
};
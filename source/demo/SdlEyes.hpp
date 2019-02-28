#pragma once
#include <iostream>
#include <math.h>
#include "../UI/UI.hpp"
using namespace std;

class SdlEyes : public UiItem
{
public:
	bool Draw() override
	{
		ImGui::Begin("Eyes", &Running, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		if (!Running)
		{
			ImGui::End();
			return false;
		}	
		Update();
		ImGui::SetCursorPos(ImVec2(X, Y));
		ImGui::Image(EyeBase, EyeBase->Size());
		ImGui::SetCursorPos(ImVec2(X + EyeW + EyeSpace, Y));
		ImGui::Image(EyeBase, EyeBase->Size());
		ImGui::SetCursorPos(ImVec2(eyeX, eyeY));
		ImGui::Image(EyeInner, EyeInner->Size());
		ImGui::SetCursorPos(ImVec2(eye2X, eyeY));
		ImGui::Image(EyeInner, EyeInner->Size());
		ImGui::End();
		return true;
	}

	SdlEyes()
	{
		EyeBase = ImGuiSDL::LoadTexture("romfs:/demo/Eye.png");
		EyeInner = ImGuiSDL::LoadTexture("romfs:/demo/EyeInner.png");

		auto eyeS = EyeBase->Size();
		EyeW = eyeS.x;
		EyeSpace = eyeS.x / 3.0;
		TotalW = EyeW * 2 + EyeSpace;
		TotalH = eyeS.y;

		EyeInnerRadius = EyeInner->Size().x / 2.0;
	}

	virtual ~SdlEyes()
	{
		ImGuiSDL::FreeTexture(EyeBase);
		ImGuiSDL::FreeTexture(EyeInner);
	}

	int TotalW;
	int TotalH;
private:
	bool Running = true;

	int eyeX = 0, eyeY = 0;
	int eye2X = 0;
	Texture *EyeBase;
	Texture *EyeInner;

	int X = 10, Y = 50;

	int EyeInnerRadius;
	int EyeW;
	int EyeSpace;

	float dist(int Ax, int Ay, int Bx, int By)
	{
		return sqrtf(powf(Ax - Bx, 2) + powf(Ay - By, 2));
	}

	void Update()
	{
		auto mouse = ImGui::GetIO().MousePos;
		int curX = mouse.x;
		int curY = mouse.y;

		auto WinPos = ImGui::GetWindowPos();

		int CenterX =  X + TotalW / 2 + WinPos.x;
		int CenterY =  Y + TotalH / 2 + WinPos.y;

		int LECenterX = CenterX - EyeInnerRadius - EyeSpace;
		int RECenterX = CenterX + EyeInnerRadius + EyeSpace;

		// Get the unit vector pointing towards the mouse position.
		float dx = curX - LECenterX;
		float dx1 = curX - RECenterX;
		float dy = curY - CenterY;
		float dist = sqrtf(dx * dx + dy * dy);
		dx /= dist;
		dx1 /= sqrtf(dx1 * dx1 + dy * dy);
		dy /= dist;

		// This point is 1/4 of the way
		// from the center to the edge of the eye.
		eyeX = LECenterX + dx * EyeW / 4.0 - EyeSpace -WinPos.x;
		eye2X = RECenterX + dx1 * EyeW / 4.0 - WinPos.x;
		eyeY = CenterY + dy * TotalH / 4.0 - WinPos.y;
	}
};
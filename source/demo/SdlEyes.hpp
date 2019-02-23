#pragma once
#include <iostream>
#include <math.h>
#include "../UI.hpp"
using namespace std;

class SdlEyes 
{
public:
	int X, Y;
	void Update(int curX, int curY)
	{
		int CenterX = X + TotalW / 2;
		int CenterY = Y + TotalH / 2;

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
		eyeX = LECenterX + dx * EyeW / 4.0 - EyeSpace;
		eye2X = RECenterX + dx1 * EyeW / 4.0;
		eyeY = CenterY + dy * TotalH / 4.0;
	}

	void Render() 
	{
		EyeBase.Render(X, Y);
		EyeBase.Render(X + EyeW + EyeSpace, Y);
		EyeInner.Render(eyeX, eyeY);
		EyeInner.Render(eye2X , eyeY);
	}

	SdlEyes(int x, int y) : EyeBase("romfs:/demo/Eye.png"), EyeInner("romfs:/demo/EyeInner.png"), X(x), Y(y)
	{
		auto eyeS = EyeBase.GetRect();
		EyeW = eyeS.w;
		EyeSpace = eyeS.w / 3.0 ;
		TotalW = EyeW * 2 + EyeSpace;
		TotalH = eyeS.h;

		EyeInnerRadius = EyeInner.GetRect().w / 2.0;
	}
	
	int TotalW;
	int TotalH;
private:
	int eyeX = 0, eyeY = 0;
	int eye2X = 0;
	Image EyeBase;
	Image EyeInner;

	int EyeInnerRadius;
	int EyeW;
	int EyeSpace;

	float dist(int Ax, int Ay, int Bx, int By)
	{
		return sqrtf(powf(Ax - Bx, 2) + powf(Ay - By, 2));
	}
};
#pragma once
#include "stdafx.h"

#ifndef _MENU_H_
#define _MENU_H_

#include <vector>
#include "KF2DTextureAnimation.h"
#include "KFButton.h"
#include "ScrollBarClass.h"
#include "StringsListClass.h"
#include "ColorPickerClass.h"
#include "FlatObjectClass.h"


using namespace std;

class Menu
{
public:
	Menu ();
   ~Menu ();

	vector<KFButton*> Buttons;
	vector<ScrollBarClass*> ScrollBars;
	vector<StringsListClass*> StringsList;
	vector<ColorPickerClass*> ColorPickers;

	FlatObjectClass * Background;

	KF2DTextureAnimation *Animation;

	int g_NumOfButtons;
	int	g_NumOfScrollBars;
	int g_NumOfStringsLists;
	int g_NumOfColorPickers;

	// ������� �� ���� - � �� �������� ������ � ��� ������
	bool IsMenuActive = 0;
	// �������� �� ����
	bool IsMenuDraw = 0;

	int ShaderForDraw = -1;

private:

};



#endif

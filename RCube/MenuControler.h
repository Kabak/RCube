#pragma once
#include "stdafx.h"

#ifndef _MENUCONTROLER_H_
#define _MENUCONTROLER_H_

#include "D3DGlobalContext.h"

#include "FPSTimers.h"
#include "InputClass.h"
#include "Menu.h"
#include "ResourceManager.h"

using namespace std;

class MenuControllerClass : public Menu {

private :

	D3DGlobalContext* McD3DGC;
	ResourceManager * GlobalResourceManager;

	InputClass* GlobalInput;

	// Позиция и размеры фона окна меню
	//	x - X в % , y - Y в % , Width в % к оригинальному размеру текстуры,
	// Height в % к оригинальному размеру текстуры
	XMFLOAT4 BackGroundCoord;	// Позиция фона 

	// Размеры меню во время анимации
//	int g_Width;
//	int g_Heigth;

	// тригер для отображения курсора в Edit
	bool CursorOn_Off;

	float CursorTime;

	// Проверка на наличие символа точки в строке для ввода FLOAT
	bool PointSet ( char* );
		
	bool KeyPressed;	// Для того чтобы кнопки нужно было отпускать и вбивалось по одному символу
	int TempKeyCode;

	// Хранится индекс последнего активного Edit
	// Нужно только для того, чтобы ВЫКЛючить курсор в предыдущем активном Edit, если там остался включённый курсор
	int LastActiveButtonIndex;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++     Buttons functions     ++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	// Меняет активный Edit на форме
	void ChangeActiveEdit( int ActiveNumber );

	// Выключаем курсор в Edit
	void SwithOFFCursor ( KFButton* Button );

// -----------------------------------------------------------------------------------------
// ------------------------------     Buttons functions     --------------------------------
// -----------------------------------------------------------------------------------------

	// Если эта переменная установлена в true , значит на одном из компонентов в меню была нажата кнопка и не отпущена
	// Объект на котором произошло нажатие должен установить этот признак в true и обязан очистить его когда мышку отпустили
	// Нужно чтобы нажатая на каком-либо объекте мышка не вызывала нажатия на других элементах меню при хаотичном движении курсора
	// по экрану с нажатой кнопкй мыши ( чтобы другие объекты не обрабатывали событие, что на нём нажали мышь

	// Устанавливается для меню, когда было активировано переопределение кнопки для игры
	bool KeyDefineExpected;

	void GetLabelBindKeyText();

	void UpdateObjectText( KFButton* TempButton, char* Str );	// Размещает текст поверх элементов меню в центе элемента по Y


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vector <MenuItem*> MenuItemsList;
	vector <GroupItem*> ItemsGroups;
	int FindMinXYPosItem ( vector <MenuItem*> &MenuItemsList, vector <GroupItem*> &ItemsGroups );
	void PushIntoGroup ( int GroupIndex, int MenuItemIndex );
	void UpdateGroupDim ( int GroupIndex, int NewItemIndex );
	void CompareItems ( MenuItem* One, MenuItem* GroupedItem, bool &Direction, bool &Identical );
	bool FindItemBetweenItems ( GroupItem* Group, MenuItem* CheckItem, bool &Direction ); // return true if its some item between One & Two items
	void CalculateMenuObjectsPositions ( vector <MenuItem*> &MenuItemsList, vector <GroupItem*> &ItemsGroups );
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

public :

	// Переопределяем операторы New и delete для правильного размещения XMMATRIX в памяти
    void* operator new(size_t i)
    {
        return _mm_malloc(i,16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }

	 MenuControllerClass();
	~MenuControllerClass();

	// _Animation - Указатель на анимацию в фоне
	// BackGroundCoord - Фактически размеры меню и позиция на экране
	HRESULT Init(D3DGlobalContext* D3DGC,
		KFButton_Elements ButtonsData[], int NumOfButtons, // Описание объектов кнопок и их количество в массиве описаний
		ScrollBar_Elements ScrollBarData[], int NumOfScrollbars,// Описание объектов ScrollBars и их количество в массиве описаний
		StringsList_Elements StringsLists[], int NumOfStringsList,// StringsList и их количество
		ColorPicker_Elements ColorPicker[], int NumOfColorPickers,	// ColorPickers
		KF2DTextureAnimation *_Animation,			// Указатель на анимацию в фоне, если NULL, то нет анимации
		XMFLOAT4 BackGroundCoord,					// Фактически размеры меню и позиция на экране
		ID3D11ShaderResourceView* BackgroundTexture,// Указатель на текстуру фона
		ResourceManager * GlobalResManager			// Указатель на класс менеджера текстур и буферов объектов
		);

	HRESULT Frame( InputClass* InputClass, int UTF16_KeyCODE, FPSTimers& fpstimers);

	// Конвертер UTF16 кода нажатой клавиши в ASCII Cyrillic char
	char UTF16toCHAR ( int UTF16_KeyPressed );

	// Конвертер UTF16 кода нажатой клавиши в ASCII Integer
	char UTF16toINT ( int UTF16_KeyPressed );

	// Конвертер UTF16 кода нажатой клавиши в ASCII Float
	char UTF16toFLOAT ( int UTF16_KeyPressed );

	// Переместить меню на позицию  ( Координаты левого верхнего угла )
	void SetMenuPos (XMFLOAT2& Pos );

	// Изменить размеры меню в % и его компонентов
	// Размер задаёся в процентах к оригинальному размеру
	void SetMenuSize ( float Percent );

	// Изменить размер и положение всем элементам меню
	void UpdateMenuElementsAll ();

	// Вернуть меню на место где оно было создано
	void SetMenuOrigin ();

	// Обновить позицию всем элементам меню
	void UpdateMenuElementsPos();

	void AddAnyMenuEffect(int Index);

	void SetMenuActive(bool Value);

	int BUSY_ObjectIndex;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++     Buttons functions     ++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	// Включить или выключить Button
	void SetButtonEnable( int ButtonIndex, bool value );
	
	// Возвращает нажата ли кнопка. Если указан несуществующий индекс Button функция возвращает false
	bool GetButtonState( int ButtonIndex );	// For CHECKBOX will be returned Checked  For other types will be returned State.IsPress
	bool GetEditFinished ( int ButtonIndex ); // true if Edit was finished in that engine Frame
	bool GetButtonChanged ( int ButtonIndex ); // For CHECKBOXes ONLY. Will be returned true if was changed last engine Frame
	bool GetScrollBarChanged ( int ScrBarIndex ); // For ScrollBars was changed 
	// Установить позицию элемента НА ФОРМЕ 
	void SetButtonPos( int buttonIndex, float x, float y );

	// Установить позицию элемента НА ФОРМЕ в процентах по отношению к ОРИГИНАЛЬНОЙ ПОЗИЦИИ этого элемента НА ФОРМЕ
	void SetButtonPos( int ButtonIndex, float Percent );

	void SetButtonSize( int ButtonIndex, float width, float heigth );

	// Установить размер элемента НА ФОРМЕ в проентах по отношению к ОРИГИНАЛЬНОМУ РАЗМЕРУ этого элемента НА ФОРМЕ
	void SetButtonSize( int ButtonIndex, float Percent );

	// Установить новые парамеметры кнопке
	void SetButtonParam( int ButtonIndex, XMFLOAT4& Param );

	// Установить кнопку в НАЧАЛЬНОЕ положение которое задавалось при её создании на форме.
	void SetButtonToOrigin( int ButtonIndex );

	// Зачем-то очищаем показания что кнопка была нажата ( не помню зачем )
	void ClearButtonState( int ButtonIndex );
	
	// Поставить в центр Label или Edit текст от привязанных к объектам предложений
	void PutLabelsInsideButtons();

	// Поставить в центр Label или Edit текст от привязанного к объекту предложения
	void PutLabelInsideButton( int ButtonIndex );

	void SetButtonAnimation( int NumOfVerses, int NumofColom, ID3D11ShaderResourceView * KF2DTextureFileName,
							 int IndexOfButton, int FramesSpead );

	float GetButtonTextAsFloat( int ButtonIndex );
	  int GetButtonTextAsInt ( int ButtonIndex );

XMFLOAT4 GetButtonColour ( int ButtonIndex );



	void SetButtonText( char* Text );

// -----------------------------------------------------------------------------------------
// ------------------------------     Buttons functions     --------------------------------
// -----------------------------------------------------------------------------------------

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++     ScrollBar functions     +++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	// Включить или выключить ScrollBar
	void SetScrollBarEnable( int ScrollBarIndex, bool value );

	// Возвращает текущее значение ScrollBar.  Если указан несуществующий индекс ScrollBar, функция возвращает -666.66666666f;
	float GetScrollBarValue( int ScrBarNumber );

	bool SetScrollBarValue( int ScrBarNumber, float &Value );

// -----------------------------------------------------------------------------------------
// -----------------------------     ScrollBar functions     -------------------------------
// -----------------------------------------------------------------------------------------
};

#endif 
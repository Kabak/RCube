#pragma once
#include "stdafx.h"

#ifndef _KFBUTTON_H_
#define _KFBUTTON_H_

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x800
#endif

#include <dinput.h>
#include "FlatObjectClass.h"
#include "D3DGlobalContext.h"
#include "DX11Buffers.h"
				
class ResourceManager;

struct ButtonState{
	ButtonState () { IsPress = false; IsMouseOnButton = false; }

	bool	IsPress;		// Button was pressed last engine Frame
	bool    IsMouseOnButton;// Mouse was on button body last engine Frame
};

class KFButton : public AboutMenuElement{

private :
		      ResourceManager* ResManager;

				  ButtonState   g_State ;

						 HWND   g_hwnd;
	ID3D11ShaderResourceView*	OsnButtontexture;
	ID3D11ShaderResourceView*	IsButonPressTexture;
	ID3D11ShaderResourceView*	IsMouseOnButtonTexture;
	ID3D11ShaderResourceView*	NotEnalbledTexture;
	
	int BodyBufferIndex;

	void ConstructBody ( XMFLOAT4 &BodyPos ); // Set the button on correct menu place

	XMFLOAT4 Colour;	// Color if the button should draw like color square

	// Для проверки, что клик и отпускание клика было на конкретном элементе меню
	// Например : Если кнопка с фиксацией - CheckBox, то отпускать мышь нужно на объекте. Иначе Checked не сработает
	// Для любого типа объекта Button : Отпускать мышку нужно на том же объекте на котором нажали кнопку мыши, иначе эффекта нажатия не произойдёт.
	int NeedReleaseOnObject; 

	// Обновление координат для вертексов
//	void UpdateBodyPos ();	// Calkulate body - vertex position on the screen
//	void ConstuctBoby ();

	// В принципе включена для манипуляций
	bool Enabled;

public :

	// if CHECKBOX
	bool Checked;		// Зафиксированна в нажатом состоянии
	bool Changed;		// true for CHECKBOX if changed this frame 
	// if EDIT
	bool ActiveAsEdit;	// Если конкретный Edit активен для ввода текста. Для отображения курсора ( показывать / не показывать )
	bool CursorOn;		// Установлено, если курсор уже отображается ( чтобы не добавлять лишний при отображении )
	bool EditFinished;  // Set true if user press Enter after Editing EDIT type  or pressed key in LABEL
	// if COLOR
	bool SetColour;		// = true If Button is COLOR type and was pressed for colorpicking.  If pressed once again Setcolour = false

	int ObjectType;		// Тип объекта которым будет работать созданный объект BUTTON = 0, CHECKBOX = 1, LABEL = 2, EDIT = 3, COLOR = 4

	int EditType;		// ANYSYMBOL = 1, AS_INTEGER = 2, AS_FLOAT = 3

	// Для Label
	bool WaitingForKeyPress;// Показывает что Label ждёт нажатия на клавишу ( для привязки кнопок к действиям в движке )
	 int VirtualKeyIndex;	// Привязка к конкретному действию в игре
    bool SecondSlot;		// Слот для определения кнопок если true, то привязка ко 2-му слоту ( два массива кнопок для дейтвий )

	// Параметры экрана
	XMFLOAT4 ScreenCoords;
	// Параметры родительской формы
	XMFLOAT4 FormCoord;

	XMFLOAT4 ObjOriginalParam;
	XMFLOAT4 ObjParam;

	FlatObjectClass *Body;

	// Абсолютные координаты для проверки нахождения мыши на объекте
	long ABSoluteX;		// Абсолютная экранная позиция по X
	long ABSoluteY;		// Абсолютная экранная позиция по Y
	long ABSolute_Width; // Абсолютная ширина
	long ABSolute_Height;// Абсолютная высота
	
	int SentenceIndex;// Индекс строки которая создана в TextClass и привязывается к этому компоненту.  
					  // Если -1, то строка отсутствует ( -1 Возможно только для Button и CheckBox. Для Label и Edit номер строки обязателен )

	 KFButton() ;
	~KFButton() ;

		// Переопределяем операторы New и delete для правильного размещения XMMATRIX в памяти
    void* operator new(size_t i)
    {
        return _mm_malloc(i,16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }
// ------
	HRESULT Init(
		D3DGlobalContext* D3DGC,
				XMFLOAT4& ScreenCoords,
				XMFLOAT4& FormCoord,
	   KFButton_Elements& ButtonInitData,
//		Flat_Obj_Buffers* _Buffers,
		ResourceManager * _GlobalResourceManager
		);


	// Возвращает true если произошла смена или активация одного из Edit на форме
	// Нужно для того, чтобы курсор мигал только в активном Edit для ввода,
	// и был активен для ввода с клавиатуры только один Edit на форме.
	// Если активный Edit не менялся то возвращается false
	// ObjectBUSY - для отслеживания нажата ли мышка на каком-либо объекте в конкретном меню
	bool Frame( DXINPUTSTRUCT& InputClass, bool &ObjectBUSY);

	void SetIfButtonPressTexture(ID3D11ShaderResourceView* Texture);

	void SetIfButtonNotEnalbledTexture(ID3D11ShaderResourceView* Texture);

	void SetIfMouseOnButtonTexture(ID3D11ShaderResourceView* Texture);

	void SetOsnTexture(ID3D11ShaderResourceView* Texture);

	void GetButtonState(ButtonState& State);

	void ClearButtonState();

	// Обновляем размер и положение Button на форме
	void ResetButtonParam();

	// Обновление данных РАЗМЕРА для чувствительности нажатия на элемент
	void UpdateABSElementSize();

	// Обновление данных ПОЗИЦИИ для чувствительности нажатия на элемент
	void UpdateABSElementPos();

	// Обновление данных и РАЗМЕРА и ПОЗИЦИИ для чувствительности нажатия на элемент
	void UpdateABSElementAll();

	// разрешает или закрывает манипуляции с объектом
	void SetEnable ( bool Value );

	void SetButtonColor ( XMFLOAT4 _Color );
	void SetButtonColorREF ( COLORREF Color );

	COLORREF GetButtonColorREF ();
	XMFLOAT4 GetButtonColor ();
};

#endif


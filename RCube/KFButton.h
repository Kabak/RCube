#pragma once
#include "stdafx.h"

#ifndef _KFBUTTON_H_
#define _KFBUTTON_H_

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x800
#endif

#include <dinput.h>
#include "FlatObjectClass.h"
#include "TextClass.h"
#include "D3DGlobalContext.h"
#include "DX11Buffers.h"
						
struct ButtonState{
	bool	IsPress;
	bool    IsMouseOnButton;
};

class KFButton : public FlatObjectClass{

private :
	
				  ButtonState   g_State ;

						 HWND   g_hwnd;
	ID3D11ShaderResourceView*	OsnButtontexture;
	ID3D11ShaderResourceView*	IsButonPressTexture;
	ID3D11ShaderResourceView*	IsMouseOnButtonTexture;
	ID3D11ShaderResourceView*	NotEnalbledTexture;
//		   ID3D11BlendState *   g_BlendState;
	
	// Для CheckBox
	bool Checked;	// Зафиксированна в нажатом состоянии

	// Для проверки, что клик и отпускание клика было на конкретном элементе меню
	// Если кнопка с фиксацией - CheckBox, то отпускать мышь нужно на объекте иначе Checked не сработает
	// Для любого объекта : Отпускать мышку нужно на том же объекте на котором нажали
	int NeedReleaseOnObject; 

	// Обновление координат для вертексов
	void SetButtonParam();

	// В принципе включена для манипуляций
	bool Enabled;

public :

	// Для Edit
	bool ActiveAsEdit;	// Если конкретный Edit активен для ввода текста. Для отображения курсора ( показывать / не показывать )
	bool CursorOn;		// Установлено, если курсор уже отображается ( чтобы не добавлять лишний при отображении )

	int ObjectType;		// Тип объекта которым будет работать созданный объект BUTTON = 0, CHECKBOX = 1, LABEL = 2, EDIT = 3

	int EditType;		// ANYSYMBOL = 1, AS_INTEGER = 2, AS_FLOAT = 3

	// Для Label
	bool WaitingForKeyPress;// Показывает что Label ждёт нажатия на клавишу
	 int VirtualKeyIndex;	// Привязка к конкретному действию в игре
    bool SecondSlot;		// Слот для определения кнопок tckb true, то привязка ко 2-му слоту

	// Параметры экрана
	XMFLOAT4 ScreenCoords;
	// Параметры родительской формы
	XMFLOAT4 FormCoord;

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
		Flat_Obj_Buffers* _Buffers
		);

	HRESULT Draw();

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

};

#endif


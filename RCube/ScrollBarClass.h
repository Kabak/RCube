#pragma once
#include "stdafx.h"

#ifndef _SCROLLBARCLASS_H_
#define _SCROLLBARCLASS_H_

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x800
#endif


#include "SquareObjectClass.h"
#include "FPSTimers.h"
#include <dinput.h>
//#include <WICTextureLoader.h>

class ScrollBarClass : public SquareObjectClass {

private:

	const float MIN_TRAVELER_SIZE = 20.0f; // В пикселях 20

	// Кнопки
	SquareObjectClass *MinButton;
	SquareObjectClass *MaxButton;
	SquareObjectClass *Traveller;

	Vertex_FlatObject *BodyVertexes;

	ID3D11ShaderResourceView *ButtonsTexture;
	ID3D11ShaderResourceView *MouseOnButtonTexture;
	ID3D11ShaderResourceView *ButtonPressTexture;
	ID3D11ShaderResourceView *NotEnalbledButtonTexture;
	ID3D11ShaderResourceView *MouseOnTravellerTexture;
	ID3D11ShaderResourceView *TravellerTexture;
	ID3D11ShaderResourceView *TravellerPressTexture;
	ID3D11ShaderResourceView *NotEnalbledTravellerTexture;
	ID3D11ShaderResourceView *BodyTexture;
	ID3D11ShaderResourceView *NotEnalbledBodyTexture;

	// Граничные значение переменной скролинга
	float MinValue;		// Минимальное значение
	float MaxValue;		// Максимальное значение
	float CurrentValue; // Текущее значение
	float StepValue;	// Шаг с которым позиционируем значения
	
//	int ClickScroll;	// Сколько значений прокручивается при нажатии на тело ScrollBar

	float MiddleBodyPos;	// Середина тела, для ровного размещения кнопок относительно центра ScrollBox

	// Нужно для отслеживания сдвига ползунка
	POINT MousePosXX;
	// Нажата одна из кнопок ScrollBar
	bool ButtonPressed;
	// Ползунок нажат
	bool TravellerPressed;
	// Нажатие на теле
	bool BodyPressed;

	// Это значение = true когда прошло нужно время после удержания мышки в нажатом 
	// состоянии на кнопках Min и Max
	bool TimerStop;
	float Time;
	

//	bool StaticTravelerWidth; // постоянная ширина ползунка

	// Коэффициент сколько данных в одном клике мышкой по телу скролбокса нужно прокрутить в процентах
	// к общему диапозону значений Min - Max в этом ScrollBox
	float Ratio;		

	// Получить перевёрнутое значение относительно центра скролбара
	float UpSideDownValue();

	// Обновление координат для вертексов
	void SetScrollBarBodyParam();
	
	// Изменяем позицию ползунка в соответствии с нажатием мышки на теле ScrollBox
	void ChangeTravelerPositionByValue( float &value );

	// Изменяем позицию ползунка по скролингу ползунка мышкой
	void ChangeTravelerPositionByScroll( long XX );

	// Устанавливаем ползунок по координатам рассчитываем по значению CurrentValue
	void BodyClickToValue( POINT &Position );

	// Получить высоту и ширину текстуры
	// Если текстура для кнопок : меньше, больше задана, и TempData z,w = 0.0f , то берём размеры текстуры
	void GetTextureParam( ID3D11ShaderResourceView * Texture, XMFLOAT4& Dimention );

	// Обработка нажатия на ползунок
	bool TravellerPreseed( DXINPUTSTRUCT& InputClass );

	XMFLOAT4 ScreenCoords;
	XMFLOAT4 FormCoord;

	// Если объект доступен для манипуляций
	bool Enabled;

public:
	// Изменяем позицию ползунка в новое значение
	void SetValue( float value );
	
	// рисуются ли кнопки по вершинам скролбара
	bool MinMixButtonsShow;
	// Скролбар позиционирует по фиксированным значениям
//	bool Jumping;
	// Горизонтальный или вертикальный скролбар
	bool Horizontal;
	// Если ScrollBar вертикальный, то удобно когда большее значение - вверху
	bool UpSideDown;


	// Абсолютные координаты для проверки нахождения мыши на объекте
	long ABSoluteX;		// Абсолютная экранная позиция по X
	long ABSoluteY;		// Абсолютная экранная позиция по Y
	long ABSolute_Width; // Абсолютная ширина
	long ABSolute_Height;// Абсолютная высота

	long MinABSoluteX;		// Абсолютная экранная позиция по X
	long MinABSoluteY;		// Абсолютная экранная позиция по Y
	long MinABSolute_Width; // Абсолютная ширина
	long MinABSolute_Height;// Абсолютная высота

	long MaxABSoluteX;		// Абсолютная экранная позиция по X
	long MaxABSoluteY;		// Абсолютная экранная позиция по Y
	long MaxABSolute_Width; // Абсолютная ширина
	long MaxABSolute_Height;// Абсолютная высота

	long TravelABSoluteX;		// Абсолютная экранная позиция по X
	long TravelABSoluteY;		// Абсолютная экранная позиция по Y
	long TravelABSolute_Width; // Абсолютная ширина
	long TravelABSolute_Height;// Абсолютная высота

	ScrollBarClass();
	~ScrollBarClass();

	// Переопределяем операторы New и delete для правильного размещения XMMATRIX в памяти
	void* operator new(size_t i)
	{
		return _mm_malloc(i,16);
	}

		void operator delete(void* p)
	{
		_mm_free(p);
	}

	HRESULT Init( D3DGlobalContext* D3DGC,
		XMFLOAT4& _ScreenCoords,
		XMFLOAT4& _FormCoord,
		KFScrollBar_Elements& TempScrollBarData
		);

	void Draw();
	
	// Возвращает номер элемента с которым были изменения
	int Frame( DXINPUTSTRUCT& InputClass, FPSTimers& fpstimers, bool &ObjectBUSY );

	void SetMouseOnButtonTexture(ID3D11ShaderResourceView * Texture);
	void SetButtonsPressTexture(ID3D11ShaderResourceView * Texture);
	void SetBarNotEnalbledButtonsTexture(ID3D11ShaderResourceView * Texture);
	void SetMouseOnTravellerTexture(ID3D11ShaderResourceView * Texture);
	void SetTravellerPressTexture(ID3D11ShaderResourceView * Texture);
	void SetTravellerNotEnalbledTexture(ID3D11ShaderResourceView * Texture);
	void SetBarNotEnalbledTexture(ID3D11ShaderResourceView * Texture);

	// Обновление данных РАЗМЕРА для чувствительности нажатия на элемент
	void UpdateABSElementSize();

	// Обновление данных ПОЗИЦИИ для чувствительности нажатия на элемент
	void UpdateABSElementPos();

	// Обновление данных и РАЗМЕРА и ПОЗИЦИИ для чувствительности нажатия на элемент
	void UpdateABSElementAll();

	void UpdateTravelerPos();

	void UpdateTravelerAll();

	// Установка кнопок и ползунка в нужную позицию по отношению к телу ScrollBox
	void SetButtonsToBody(bool _Horizontal, XMFLOAT4 &MinButton, XMFLOAT4 &MaxButton, XMFLOAT4 &TravellerButton );

	float GetCurrentValue();

	bool CheckSign( float Val );

	float GetValuesDiaposone();

	// разрешает или закрывает манипуляции с объектом
	void SetEnable ( bool Value );
};

#endif
#pragma once
#include "stdafx.h"

#ifndef _STRINGSLISTCLASS_H_
#define _STRINGSLISTCLASS_H_

#include "FlatObjectClass.h"
#include "FPSTimers.h"
#include "D3DGlobalContext.h"
#include <vector>

class ResourceManager;

class StringsListClass : public AboutMenuElement
{

public:
	
	StringsListClass();
   ~StringsListClass();

	void ShutDown ();

	bool ShowStringsList;

	bool Enabled;

	// Параметры родительской формы
	XMFLOAT4 FormCoord;

	// Абсолютные координаты для проверки нахождения мыши на объекте
	long ABSoluteX;		// Абсолютная экранная позиция по X
	long ABSoluteY;		// Абсолютная экранная позиция по Y
	long ABSolute_Width; // Абсолютная ширина
	long ABSolute_Height;// Абсолютная высота

//	vector <ID3D11ShaderResourceView*> StringsList_SRV; // Массив текстур с написаным текстом для текстовых строк из StringsList

	int StringsNumber;		// Количество строк в StringList

	int MaxVisibleIndex;	// Сколько строк рисовать из общего списка

	FlatObjectClass* StringListObj;

	// Статический имеет ограничения по количеству элементов - строк в списке. 
	// но рисуется быстрее. За счёт того, что строки рисуются на текстуре один раз без возможности обновления 
	HRESULT Init( D3DGlobalContext* D3DGC,
			   XMFLOAT4& _FormCoord,
			   StringsList_Elements& StringsListInit,
			   ResourceManager * _GlobalResourceManager
			 );
																	
	bool SaveTextureToPNG( ID3D11ShaderResourceView* );

	void UpdateABSElementAll();

	void UpdateBodyPos();	// Calkulate body - vertex position on the screen

	// Возвращает номер элемента с которым были изменения
	int Frame( DXINPUTSTRUCT& InputClass, FPSTimers& fpstimers, bool &ObjectBUSY );

	// Сдвигаем все строки вверх
	void ScrollUp();
	// Сдвигаем все строки вниз
	void ScrollDown();

	int SentencesIndex;			// Индекс группы текстовых строк в списке всех строк резервируется для конкретнго StringsList, чтобы все строки для конкретного StringsList рисовались одновременно

private:
	
	D3DGlobalContext* Local_D3DGC;
	ResourceManager* ResManager;

	XMFLOAT4 ObjOriginalParam;
	XMFLOAT4 ObjParam;

	int StringListObjBufferIndex;

	vector <char*> Strings;	// Массив строк подлежащих отрисовке

vector <int> SentencesResIndex;// Список индексов предложений зарезервированных для нашего StringsList

	int FontIndex;				// номер шрифта которым рисуется текст
	int FontHeightInPixel;		// Размер текстуры шрифта в пикселях ( для качественного написания любого символа )

	int Strings_Amount_ToRender;// Сколько всего строк нужно рисовать

	int First_Visible_Index;	// номер 1-й отображаемой строки в общем списке строк
	int Last_Visible_Index;		// номер последней отображаемой строки в общем списке строк

	int StringHeigt;			// Высота строки текста в пикселях
	int StringsSpacing;			// расстояние между строками
  float Scaling;				// Для подгонки текста в окно StringsList

	int Scroll_Y_Pos;			// Вверхняя левая Позиция массива строк при скролинге. Относительно которой рисуются все строки
	int ScrollSpeed;			// Скорость прокрутки
	int StringsHeigh;			// Высота одной строки в StringsKist

	int ObjParam_X;				// int версия массива ObjParam для ускорения работы с позицией на экране в int
	int ObjParam_Y;
	int ObjParam_Z;
	int ObjParam_W;

   bool StringsListPressed;		// Левая кнопка мыши нажата на StringsList
    int Old_MouseZ;				// Старое значение 

	int StringsMAXLength;		// Максимальная длинна любой строки

	XMFLOAT2 FirstStringOffset; // Сдвиг строк текста относительно угла

	// Координаты в текстуре для отображения куска
	XMFLOAT4 TextureLeftTop;
	XMFLOAT4 TextureRightBottom;
	XMFLOAT4 TextureLeftBottom;
	XMFLOAT4 TextureRightTop;

	XMFLOAT4 StrList_Frame_pos;			// Позиция окна в текстуре где мы рисуем строки текста ( для вырезания этого куска в реальное окно )

	// ScrollDir = 1 - Up, = 0 - Down
	void ScrollStringsList( bool ScrollDir ); // Проверка на попадание строки в зону видимости

	void UpdateVisibleSentence( bool ScrollDir );	// Изменяем видимые строки в StringsList

	bool SetInitFrameData();	// Установнка начальных значений для окна в котором будут отображаться строки ( зависит от высоты строк и промежутком между строками )

	void UpdateVisibleSentance();	// Обновляем данные для всех видимых на экране предложений

	ID3D11RenderTargetView* Default_RTV;// Основной экран
	ID3D11RenderTargetView* StrList_RTV;// Текстура куда рисуется StringsList который нужно прокручивать в окошке
  ID3D11ShaderResourceView* StrList_SRV;// Для сохранения в файл и отображения строк в меню
};

#endif
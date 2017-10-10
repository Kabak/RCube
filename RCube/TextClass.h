#pragma once
#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////
// Filename: textclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTCLASS_H_
#define _TEXTCLASS_H_

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "D3DGlobalContext.h"
#include "FontClass.h"
#include "FontShaderClass.h"
#include "DirectXMath.h"
#include "FPSTimers.h"
#include "ResourceManager.h"
#include <vector>

using namespace std;
using namespace DirectX;
// Максимальный размер быстрого предложения - строки текста
const UINT STRMAXLENGTH = 256;

// Перечисление разных типов отрисовки предложений
typedef enum SHOW_TEXT {
				SHOW			= 0, // просто отображение без эфектов
				SHOW_FPS		= 1, // NOT IMPLEMENTED YET
				SHOW_GLOWING	= 2, // моргающая строка
				SHOW_SCROLLING	= 3, // движущаяся строка 
} ShowMethod;

// Перечисление разных типов убирания предложений с экрана
typedef enum HIDE_TEXT {
				HIDE			= 0, // просто отключить без эфектов

} HideMethod;
////////////////////////////////////////////////////////////////////////////////
// Class name: TextClass
////////////////////////////////////////////////////////////////////////////////
class TextClass
{
private:

// SentenceType is the structure that holds the rendering information for each text sentence.

	struct SentenceType
	{
	ID3D11Buffer*	vertexBuffer;
	ID3D11Buffer*	indexBuffer;
		XMVECTOR	Colour;		// цвет и alpha
		XMFLOAT4	ColourTemp; // Временное хранение цвета
		XMFLOAT4	ColourAction;// переменная для работы с цветом
		   float	TempValShow;// Временная переменная для работы Show
		   float	TempValHide;// Временная переменная для работы Hide
		   float	TempValScroll; // Временная переменная для скролинга
		   float	ScrollSpeed; // Скорость сдвига предложения
			char	Text[STRMAXLENGTH];
			 int	vertexCount;
			 int	indexCount;
			 int	MaxLength;	// максимальная длинна предложения
			 int	FontType;	// номер шрифта из созданных пользователем ( индекс шрифта в массиве векторов )
			 int	ShowType;	// как именно отрисовывается, анимируется на экране ( разные шейдеры и функции )
			 int	HideType;	// как именно убирается с экрана при установке render = false ( разные шейдеры и функции )
			 int	PosX;		// позиция X на экране
			 int	PosY;		// позиция Y на экране
			 int	Level;		// 0 - текст в игре; 1 - HUD; 2 - Menu
			bool	Render;		// рисовать ли предложение на экране. False = исключается из списка в массиве отрисовываемых
			bool	ShowGoOn;	// Анимация действует
			bool	HideGoOn;	// Убирание предложения действует
			bool	ScrollGoOn;	// Используется для скролинга предложения
			UINT	Height;		// Высота предложения в пикселях
	};
// Количество созданных предложений
// Если = -1 , значит предложений в списке нет
	int SentecesCounter;

// The VertexType must match the one in the FontClass.
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	int TextShaderIndex = -1;	// Индекс шейдера для рисования текста в массиве шейдеров

	FPSTimers fpstimers;
// Переопределяем операторы New и delete для правильного размещения XMMATRIX в памяти
    void* operator new(size_t i)
    {
        return _mm_malloc(i,16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }
// ------------------------------------

	bool Initialize(D3DGlobalContext* D3DGC, vector < FontOnTextureData* > FontList );
	void Shutdown();

	// Добавляет предложение и возвращает его номер в списке или -1 если не удалось добавить предложение
	int AddSentence( SENTENCE_INIT_DATA* data, char* String );

	// Вытирает конкретное предложение
	int DeleteSentence( int SentenceNumber );

	// Вытирает все предложения
	void DeleteAllSentences();

	// Отрисовать предложение методом
	void ShowSentence( int SentenceNumber, int ShowMethod );

	// Изменить метод отрисовки предложения
	void ChangeShowMethod ( int SentenceNumber, int ShowMethod );

	// Убрать предложение с экрана методом
	void HideSentence( int SentenceNumber, int HideMethod );

	// Изменить метод убирания предложения
	void ChangeHideMethod ( int SentenceNumber, int HideMethod );

	// Установить конкретному предложению - рисовать ли его Switch = true - да рисовать.
	void ChangeRender ( int SentenceNumber, bool Switch );

	// Получить максимальную длинну предложения
	int GetSentenceMaxLength ( int SentenceNumber );

	// Получить высоту текста в предложения в пикселях
	UINT GetSentenceHeight ( int SentenceNumber );

	// Получить высоту текстуры шрифта в пикселях
	int GetFontHeightInPixels( int FontNumber );

	// Получить текущую длинну предложения
	UINT GetSentenceLength ( int SentenceNumber );

	// Получить указатель на строку
	char* GetSentenceText ( int SentenceNumber );

	// Изменить цвет и прозрачность
	void ChangeSentenceColour( int SentenceNumber, float Red, float Green, float Blue, float Alpha );

	int GetPosX( int SentenceNumber );

	int GetPosY( int SentenceNumber );

	// Обновляет текст и позицию предложения на экране
	bool UpdateSentence( int SentenceNumber, char* text, int positionX, int positionY, float RenderFontSize = 36.0f );

	// Рисует все предложения которые есть в списке и у них включен Render
	bool Render( int Level );

	bool SetMousePosition(int&, int&);

	bool SetFps(int fps);
	bool SetCpu(int);

	// Список доступных шрифтов
	vector < FontClass* > m_Font;
	// Список текстовых предложений для отрисовки
	vector < SentenceType* > Sentences;

	void DrawOneSentence( int SentenceIndex );

private:
	bool InitializeSentence( SentenceType*, int& maxLength );
	bool RenderSentence( SentenceType* );

// Функции отрисовки текста
//	1 - SHOW_FPS
	void ShowFPS ();
//	2 - SHOW_GLOWING
	void ShowGlowing (int Number);
	void ShowScrolling( int Number );

	D3DGlobalContext* Local_D3DGC;

	FontShaderClass* m_FontShader;
	int m_screenWidth, m_screenHeight;

	char *mouseString;
	char *tempString;
	char *fpsString;
	char *cpuString;
	
	char *Triangel1;
	char *Triangel2;
	char *Cube1;
	char *Cube2;
	char *Gran;
	char *Side;

	UINT FontsNumber;
	UINT FontsCreated;
};

#endif

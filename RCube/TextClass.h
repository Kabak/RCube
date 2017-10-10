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
// ������������ ������ �������� ����������� - ������ ������
const UINT STRMAXLENGTH = 256;

// ������������ ������ ����� ��������� �����������
typedef enum SHOW_TEXT {
				SHOW			= 0, // ������ ����������� ��� �������
				SHOW_FPS		= 1, // NOT IMPLEMENTED YET
				SHOW_GLOWING	= 2, // ��������� ������
				SHOW_SCROLLING	= 3, // ���������� ������ 
} ShowMethod;

// ������������ ������ ����� �������� ����������� � ������
typedef enum HIDE_TEXT {
				HIDE			= 0, // ������ ��������� ��� �������

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
		XMVECTOR	Colour;		// ���� � alpha
		XMFLOAT4	ColourTemp; // ��������� �������� �����
		XMFLOAT4	ColourAction;// ���������� ��� ������ � ������
		   float	TempValShow;// ��������� ���������� ��� ������ Show
		   float	TempValHide;// ��������� ���������� ��� ������ Hide
		   float	TempValScroll; // ��������� ���������� ��� ���������
		   float	ScrollSpeed; // �������� ������ �����������
			char	Text[STRMAXLENGTH];
			 int	vertexCount;
			 int	indexCount;
			 int	MaxLength;	// ������������ ������ �����������
			 int	FontType;	// ����� ������ �� ��������� ������������� ( ������ ������ � ������� �������� )
			 int	ShowType;	// ��� ������ ��������������, ����������� �� ������ ( ������ ������� � ������� )
			 int	HideType;	// ��� ������ ��������� � ������ ��� ��������� render = false ( ������ ������� � ������� )
			 int	PosX;		// ������� X �� ������
			 int	PosY;		// ������� Y �� ������
			 int	Level;		// 0 - ����� � ����; 1 - HUD; 2 - Menu
			bool	Render;		// �������� �� ����������� �� ������. False = ����������� �� ������ � ������� ��������������
			bool	ShowGoOn;	// �������� ���������
			bool	HideGoOn;	// �������� ����������� ���������
			bool	ScrollGoOn;	// ������������ ��� ��������� �����������
			UINT	Height;		// ������ ����������� � ��������
	};
// ���������� ��������� �����������
// ���� = -1 , ������ ����������� � ������ ���
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

	int TextShaderIndex = -1;	// ������ ������� ��� ��������� ������ � ������� ��������

	FPSTimers fpstimers;
// �������������� ��������� New � delete ��� ����������� ���������� XMMATRIX � ������
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

	// ��������� ����������� � ���������� ��� ����� � ������ ��� -1 ���� �� ������� �������� �����������
	int AddSentence( SENTENCE_INIT_DATA* data, char* String );

	// �������� ���������� �����������
	int DeleteSentence( int SentenceNumber );

	// �������� ��� �����������
	void DeleteAllSentences();

	// ���������� ����������� �������
	void ShowSentence( int SentenceNumber, int ShowMethod );

	// �������� ����� ��������� �����������
	void ChangeShowMethod ( int SentenceNumber, int ShowMethod );

	// ������ ����������� � ������ �������
	void HideSentence( int SentenceNumber, int HideMethod );

	// �������� ����� �������� �����������
	void ChangeHideMethod ( int SentenceNumber, int HideMethod );

	// ���������� ����������� ����������� - �������� �� ��� Switch = true - �� ��������.
	void ChangeRender ( int SentenceNumber, bool Switch );

	// �������� ������������ ������ �����������
	int GetSentenceMaxLength ( int SentenceNumber );

	// �������� ������ ������ � ����������� � ��������
	UINT GetSentenceHeight ( int SentenceNumber );

	// �������� ������ �������� ������ � ��������
	int GetFontHeightInPixels( int FontNumber );

	// �������� ������� ������ �����������
	UINT GetSentenceLength ( int SentenceNumber );

	// �������� ��������� �� ������
	char* GetSentenceText ( int SentenceNumber );

	// �������� ���� � ������������
	void ChangeSentenceColour( int SentenceNumber, float Red, float Green, float Blue, float Alpha );

	int GetPosX( int SentenceNumber );

	int GetPosY( int SentenceNumber );

	// ��������� ����� � ������� ����������� �� ������
	bool UpdateSentence( int SentenceNumber, char* text, int positionX, int positionY, float RenderFontSize = 36.0f );

	// ������ ��� ����������� ������� ���� � ������ � � ��� ������� Render
	bool Render( int Level );

	bool SetMousePosition(int&, int&);

	bool SetFps(int fps);
	bool SetCpu(int);

	// ������ ��������� �������
	vector < FontClass* > m_Font;
	// ������ ��������� ����������� ��� ���������
	vector < SentenceType* > Sentences;

	void DrawOneSentence( int SentenceIndex );

private:
	bool InitializeSentence( SentenceType*, int& maxLength );
	bool RenderSentence( SentenceType* );

// ������� ��������� ������
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

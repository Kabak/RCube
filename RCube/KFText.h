#ifndef _KFTEXT_H_
#define _KFTEXT_H_

#include <Windows.h>
#include <d3d11.h>
#include "fontclass.h"
#include "fontshaderclass.h"
#include "KFTextureManeger.h"

class KFTextClass
{
public:
	//KFTextClass();
	~KFTextClass();

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

	bool  IsFontFull ;

	HRESULT Init(HWND hwnd , ID3D11Device* device , ID3D11DeviceContext* deviceContext , XMMATRIX& baseViewMatrix , int screenWidth , int screenHeight , int ScreenPosX , int ScreenPosY ,
						  ID3D11RenderTargetView * ActiveRenderTarget , IDXGIAdapter1 * Adapter , wchar_t * FontName , UINT FontSize , XMFLOAT4 Color , XMMATRIX & OrthoMatrix);
	void Render(char* Text , float Red , float Green , float Blue, int x , int y , XMMATRIX &worldMatrix, XMMATRIX &orthoMatrix) ;
private:

		struct SentenceType
	{
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

		struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

    int m_screenWidth , m_screenHeight ;
	FontClass* m_Font;
	FontShaderClass* m_FontShader;
	XMMATRIX m_baseViewMatrix;
	SentenceType* m_sentence1;
    ID3D11DeviceContext* g_deviceContext ;
	KFTextureManeger* TextureManeger ;

	bool RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, XMMATRIX &worldMatrix, 
			       XMMATRIX &orthoMatrix);
	bool UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green, float blue,
			       ID3D11DeviceContext* deviceContext);
	bool InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device) ;
    void ReleaseSentence(SentenceType** sentence);
	void SetFontData( XMMATRIX & OrthoMatrix ) ;
	 
};

#endif
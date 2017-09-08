#pragma once
#include "stdafx.h"

#ifndef _KFSKYDOMESCLASS_H_
#define _KFSKYDOMESCLASS_H_

#include <fstream>
#include <DirectXMath.h>
#include <d3d11.h>
#include "D3Dcompiler.h"
#include "D3DGlobalContext.h"
//#include <d3dx11async.h>

using namespace DirectX;

class KFSkyDomesClass
{
public:
	~KFSkyDomesClass() ;

	// Переопределяем операторы New и delete для правильного размещения XMMATRIX в памяти
    void* operator new(size_t i)
    {
        return _mm_malloc(i,16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }
// -----------------------------------
	HRESULT Init(HWND hwnd ,ID3D11Device* device, ID3D10Blob * Blob) ;
	void Render(ID3D11DeviceContext* DeviceContext , XMMATRIX& worldMatrix, XMMATRIX& viewMatrix , XMMATRIX& projectionMatrix);

private:
		struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VertexType
	{
		XMFLOAT3 position;
	};

	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct GradientBufferType
	{
		XMFLOAT4 apexColor;
		XMFLOAT4 centerColor;
	};

	ModelType* m_model;
	int m_vertexCount, m_indexCount;
	XMFLOAT4 m_apexColor, m_centerColor;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_gradientBuffer;

	bool LoadSkyDomeModel(char* filename);
	void RenderBuffers(ID3D11DeviceContext* deviceContext);
	bool InitializeBuffers(ID3D11Device* device);
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX& worldMatrix, XMMATRIX& viewMatrix,
											 XMMATRIX& projectionMatrix, XMFLOAT4& apexColor, XMFLOAT4& centerColor);
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename, ID3D10Blob * Blob);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);
};

#endif
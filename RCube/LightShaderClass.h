#include "stdafx.h"
// The new LightShaderClass is just the TextureShaderClass from the previous tutorials re-written slightly to incorporate lighting.
////////////////////////////////////////////////////////////////////////////////
// Filename: lightshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHTSHADERCLASS_H_
#define _LIGHTSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include "DirectXMath.h"
#include "DirectXPackedVector.h"
#include "D3Dcompiler.h"
#include <fstream>

using namespace std;
using namespace DirectX;


////////////////////////////////////////////////////////////////////////////////
// Class name: LightShaderClass
////////////////////////////////////////////////////////////////////////////////
class LightShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

// The new LightBufferType structure will be used for holding lighting information. This typedef is the same as the new typedef in the pixel shader. Do note that I add a extra float for size 
// padding to ensure the structure is a multiple of 16. Since the structure without an extra float is only 28 bytes CreateBuffer would have failed if we used a sizeof(LightBufferType) 
// because it requires sizes that are a multiple of 16 to succeed.
// We add a new camera buffer structure to match the new camera constant buffer in the vertex shader. Note we add a padding to make the structure size a multiple of 16 to prevent CreateBuffer 
// failing when using sizeof with this structure.

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};

// The LightBufferType has been modified to hold a specular color and specular power to match the light constant buffer in the pixel shader. Pay attention to the fact that I placed the 
// specular power by the light direction to form a 4 float slot instead of using padding so that the structure could be kept in multiples of 16 bytes. Also had specular power been placed last 
// in the structure and no padding used beneath light direction then the shader would not have functioned correctly. This is because even though the structure was a multiple of 16 the individual 
// slots themselves were not aligned logically to 16 bytes each.

	struct LightBufferType
	{
		XMVECTOR ambientColor;
		XMVECTOR diffuseColor;
		XMVECTOR specularColor;
		XMFLOAT3 lightDirection;
		float specularPower;
	};

public:
	LightShaderClass();
	LightShaderClass(const LightShaderClass&);
	~LightShaderClass();

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

	bool Initialize(ID3D11Device*, HWND , ID3D10Blob*);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX&, XMMATRIX&, XMMATRIX&, ID3D11ShaderResourceView*, XMFLOAT3&, XMVECTOR&, XMVECTOR&, XMFLOAT3&, XMVECTOR&,float&, ID3D10Blob*);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*, ID3D10Blob* );
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX&, XMMATRIX&, XMMATRIX&, ID3D11ShaderResourceView*, XMFLOAT3&, XMVECTOR&, XMVECTOR&, XMFLOAT3&, XMVECTOR&,float&);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;

// There is a new private constant buffer for the light information (color and direction). The light buffer will be used by this class to set the global light variables inside the HLSL pixel shader.
// We add a new camera constant buffer here which will be used for setting the camera position in the vertex shader.

	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_lightBuffer;
};

#endif

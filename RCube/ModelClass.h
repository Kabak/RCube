#include "stdafx.h"

// As stated previously the ModelClass is responsible for encapsulating the geometry for 3D models. In this tutorial we will manually setup the data for a single green triangle. We will also create
// a vertex and index buffer for the triangle so that it can be rendered.
////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include "DirectXMath.h"
#include "DirectXPackedVector.h"
#include "WICTextureLoader.h"

#include <fstream>
using namespace std;
using namespace DirectX;
///////////////////////
// MY CLASS INCLUDES //
///////////////////////

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass
{
private:
// Here is the definition of our vertex type that will be used with the vertex buffer in this ModelClass. Also take note that this typedef must match the layout in the ColorShaderClass 
// that will be looked at later in the tutorial.

// The next change is the addition of a new structure to represent the model format. It is called ModelType. It contains position, texture, and normal vectors the same as our file format does.

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();


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

// The Initialize function will now take as input the character string file name of the model to be loaded.

	bool Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext * devCon, char* modelFilename, WCHAR* textureFilename);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	int GetVertexCount();

// The ModelClass also has a GetTexture function so it can pass its own texture resource to shaders that will draw this model.
	ID3D11ShaderResourceView* GetTexture();

	// моя функция для получения информации о вертексах +++++++++++++++++++++++++++++++++++++++++++++++++++++++++KF
	void GetVertexType(VertexType * Vertexes) ;
	// моя функция для получения информации о вертексах --------------------------------------------------------KF
	// моя функция для получения информации о индексах +++++++++++++++++++++++++++++++++++++++++++++++++++++++++KF
	void GetIndexes(unsigned long* Indexes) ;
	// моя функция для получения информации о индексах --------------------------------------------------------KF
private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

// ModelClass has both a private LoadTexture and ReleaseTexture for loading and releasing the texture that will be used to render this model.
	bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext * devcon, WCHAR* filename);
	void ReleaseTexture();
// We also have two new functions to handle loading and unloading the model data from the text file.

	bool LoadModel(char*);
	void ReleaseModel();


// The private variables in the ModelClass are the vertex and index buffer as well as two integers to keep track of the size of each buffer. Note that all DirectX 11 buffers generally use 
// the generic ID3D11Buffer type and are more clearly identified by a buffer description when they are first created.
private:
	ID3D11Buffer *m_vertexBuffer;
	ID3D11Buffer *m_indexBuffer;
	ID3D11ShaderResourceView * m_ShaderTexture;
	int m_vertexCount, m_indexCount;

// The m_Texture variable is used for loading, releasing, and accessing the texture resource for this model.
	ModelType* m_model;
	VertexType* vertices;
	unsigned long* indices;
};

#endif

#pragma once
#include "stdafx.h"

#ifndef _DX11BUFFERS_H_
#define _DX11BUFFERS_H_

#include <vector>

#ifdef RCube_DX11
#include <d3d11.h>
#include "Buffers_def.h"
#include <vector>

typedef ID3D11Buffer DXBuffer;
typedef ID3D11ShaderResourceView DXTextureSRV;

static const wchar_t* DX11BuffersError = L"ResourceManager Error" ;

// + ConstantBuffer
template <typename T>
class ConstantBuffer
{
public:
	ConstantBuffer ( ID3D11Device* d3dDevice, ID3D11DeviceContext* D3DContext, bool _CPUAccess );
   ~ConstantBuffer ();

   DXBuffer* GetBuffer () { return Buffer; }

	inline T* MapDiscard ( );
	void Unmap ( );
	void Update ( T* Var );

	DXBuffer* Buffer;
	ID3D11DeviceContext* DX11Context;

private:

	bool CPU_Access;
	UINT BufferSize;
};


template <typename T>
ConstantBuffer<T>::ConstantBuffer ( ID3D11Device* d3dDevice, ID3D11DeviceContext* D3DContext, bool _CPUAccess)
{
#ifdef RCube_DX11

		BufferSize = sizeof ( T );

		CD3D11_BUFFER_DESC desc (
			BufferSize,// * Amount,
			D3D11_BIND_CONSTANT_BUFFER,
			_CPUAccess ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT,
			_CPUAccess ? D3D11_CPU_ACCESS_WRITE : 0,
			0,	// Misc Flags
			0	// Structure Bytes Stride
		);

	HRESULT hr = d3dDevice->CreateBuffer ( &desc, 0, &Buffer );
	if (FAILED ( hr ))
	{
		MessageBox ( 0, L"Could not create constant buffer.", DX11BuffersError, MB_OK );
	}

	CPU_Access = _CPUAccess;
	DX11Context = D3DContext;

#endif //RCube_DX11
};

template <typename T>
ConstantBuffer<T>::~ConstantBuffer ()
{
	RCUBE_RELEASE ( Buffer );
}

#ifdef RCube_DX11
template <typename T>
T* ConstantBuffer<T>::MapDiscard ()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	DX11Context->Map ( Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
	return (T*)(mappedResource.pData);
}

template <typename T>
void ConstantBuffer<T>::Unmap ()
{
	DX11Context->Unmap ( Buffer, 0 );
}

template <typename T>
void ConstantBuffer<T>::Update ( T* Var )
{
	if (CPU_Access)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		DX11Context->Map ( Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
		T* Temp = (T*)(mappedResource.pData);
		memcpy ( Temp, Var, BufferSize );
		DX11Context->Unmap ( Buffer, 0 );
	}
	else
	{
		DX11Context->UpdateSubresource ( Buffer, 0, 0, Var, 0, 0 );
	}
}
#endif //RCube_DX11
// - ConstantBuffer




// + VertexBuffer
template <typename T>
class VertexBuffer
{
public:
	VertexBuffer ( ID3D11Device* d3dDevice, ID3D11DeviceContext* D3DContext, bool CPUAccess, UINT InstanceAmount );
	~VertexBuffer ();

	DXBuffer* GetBuffer () { return Buffer; }

	inline T* MapDiscard ();
	void Unmap ();
	void Update ( T* Var );

	//private:
	DXBuffer* Buffer;
	ID3D11DeviceContext* DX11Context;
	UINT InstanceAmount;

private:

	bool CPU_Access;
	UINT BufferSize;

};


template <typename T>
VertexBuffer<T>::VertexBuffer ( ID3D11Device* d3dDevice, ID3D11DeviceContext* D3DContext, bool _CPUAccess, UINT _InstanceAmount )
{
#ifdef RCube_DX11
	InstanceAmount = _InstanceAmount;
	BufferSize = sizeof ( T ) * InstanceAmount;
	CD3D11_BUFFER_DESC desc (
		BufferSize,
		D3D11_BIND_VERTEX_BUFFER,
		_CPUAccess ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT,
		_CPUAccess ? D3D11_CPU_ACCESS_WRITE : 0,
		0,	// Misc Flags
		0	// Structure Bytes Stride
	);

	HRESULT hr = d3dDevice->CreateBuffer ( &desc, 0, &Buffer );
	if (FAILED ( hr ))
	{
		MessageBox ( 0, L"Could not create Vertex buffer.", DX11BuffersError, MB_OK );
	}

	CPU_Access = _CPUAccess;

	DX11Context = D3DContext;
#endif //RCube_DX11
};

template <typename T>
VertexBuffer<T>::~VertexBuffer ()
{
	RCUBE_RELEASE ( Buffer );
}

#ifdef RCube_DX11
template <typename T>
T* VertexBuffer<T>::MapDiscard ( )
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	DX11Context->Map ( Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
	return (T*)(mappedResource.pData);
}

template <typename T>
void VertexBuffer<T>::Unmap ( )
{
	DX11Context->Unmap ( Buffer, 0 );
}

template <typename T>
void VertexBuffer<T>::Update ( T* Var )
{
	if (CPU_Access)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		DX11Context->Map ( Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
		T* Temp = (T*)(mappedResource.pData);
		memcpy ( Temp, Var, BufferSize );
		DX11Context->Unmap ( Buffer, 0 );
	}
	else
	{
		DX11Context->UpdateSubresource ( Buffer, 0, 0, Var, 0, 0 );
	}
}
#endif //RCube_DX11
// - VertexBuffer



// + IndexBuffer
template <typename T>
class IndexBuffer
{
public:
	IndexBuffer ( ID3D11Device* d3dDevice, ID3D11DeviceContext* D3DContext, bool _CPUAccess, UINT Amount );
	~IndexBuffer ();

	DXBuffer* GetBuffer () { return Buffer; }

	inline T* MapDiscard ();
	void Unmap ();
	void Update ( T* Var );

	//private:
	DXBuffer* Buffer;
	ID3D11DeviceContext* DX11Context;

private:

	bool CPU_Access;
	UINT BufferSize;
};


template <typename T>
IndexBuffer<T>::IndexBuffer ( ID3D11Device* d3dDevice, ID3D11DeviceContext* D3DContext, bool _CPUAccess, UINT Amount )
{
#ifdef RCube_DX11
	BufferSize = sizeof ( T ) * (size_t)Amount;
	CD3D11_BUFFER_DESC desc (
		BufferSize,
		D3D11_BIND_INDEX_BUFFER,
		_CPUAccess ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT,
		_CPUAccess ? D3D11_CPU_ACCESS_WRITE : 0,
		0,	// Misc Flags
		0	// Structure Bytes Stride
	);

	HRESULT hr = d3dDevice->CreateBuffer ( &desc, 0, &Buffer );
	if (FAILED ( hr ))
	{
		MessageBox ( 0, L"Could not create Index buffer.", DX11BuffersError, MB_OK );
	}

	DX11Context = D3DContext;
	CPU_Access = _CPUAccess;

#endif //RCube_DX11
};

template <typename T>
IndexBuffer<T>::~IndexBuffer ()
{
	RCUBE_RELEASE ( Buffer );
}

#ifdef RCube_DX11
template <typename T>
T* IndexBuffer<T>::MapDiscard ( )
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	DX11Context->Map ( Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
	return (T*)(mappedResource.pData);
}

template <typename T>
void IndexBuffer<T>::Unmap ( )
{
	DX11Context->Unmap ( Buffer, 0 );
}

template <typename T>
void IndexBuffer<T>::Update ( T* Var )
{
	if (CPU_Access)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		DX11Context->Map ( Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
		T* Temp = (T*)(mappedResource.pData);
		memcpy ( Temp, Var, BufferSize );
		DX11Context->Unmap ( Buffer, 0 );
	}
	else
	{
		DX11Context->UpdateSubresource ( Buffer, 0, 0, Var, 0, 0 );
	}
}
#endif //RCube_DX11
// - IndexBuffer


// + StructuredBuffer
// NOTE: Ensure that T is exactly the same size/layout as the shader structure!
template <typename T>
class StructuredBuffer
{
public:
	// Construct a structured buffer
	StructuredBuffer ( ID3D11Device* d3dDevice, int elements,
		UINT bindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
		bool dynamic = false );

	~StructuredBuffer ();

	DXBuffer* GetBuffer () { return mBuffer; }
	ID3D11UnorderedAccessView* GetUnorderedAccess () { return mUnorderedAccess; }
	DXTextureSRV* GetShaderResource () { return mShaderResource; }

	// Only valid for dynamic buffers
	// TODO: Support NOOVERWRITE ring buffer?
	inline T* MapDiscard ( ID3D11DeviceContext* d3dDeviceContext );
	void Unmap ( ID3D11DeviceContext* d3dDeviceContext );

private:
	// Not implemented
	StructuredBuffer ( const StructuredBuffer& );
	StructuredBuffer& operator=( const StructuredBuffer& );

	int mElements;
	DXBuffer* mBuffer;
	DXTextureSRV* mShaderResource;
	ID3D11UnorderedAccessView* mUnorderedAccess;
};


template <typename T>
StructuredBuffer<T>::StructuredBuffer ( ID3D11Device* d3dDevice, int elements,
	UINT bindFlags, bool dynamic )
	: mElements ( elements ), mShaderResource ( 0 ), mUnorderedAccess ( 0 )
{
	CD3D11_BUFFER_DESC desc ( sizeof ( T ) * elements, bindFlags,
		dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT,
		dynamic ? D3D11_CPU_ACCESS_WRITE : 0,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
		sizeof ( T ) );

	HRESULT res = d3dDevice->CreateBuffer ( &desc, 0, &mBuffer );
	assert ( res == S_OK );
	if (bindFlags & D3D11_BIND_UNORDERED_ACCESS) {
		res = d3dDevice->CreateUnorderedAccessView ( mBuffer, 0, &mUnorderedAccess );
		assert ( res == S_OK );
	}

	if (bindFlags & D3D11_BIND_SHADER_RESOURCE) {
		res = d3dDevice->CreateShaderResourceView ( mBuffer, 0, &mShaderResource );
		assert ( res == S_OK );
	}
}


template <typename T>
StructuredBuffer<T>::~StructuredBuffer ()
{
	RCUBE_RELEASE ( mUnorderedAccess);
	RCUBE_RELEASE ( mShaderResource);
	RCUBE_RELEASE ( mBuffer);
}


template <typename T>
T* StructuredBuffer<T>::MapDiscard ( ID3D11DeviceContext* d3dDeviceContext )
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	d3dDeviceContext->Map ( mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
	return (T*)(mappedResource.pData);
}


template <typename T>
void StructuredBuffer<T>::Unmap ( ID3D11DeviceContext* d3dDeviceContext )
{
	d3dDeviceContext->Unmap ( mBuffer, 0 );
}
// - StructuredBuffer


class Flat_Obj_Buffers
{
public:
	Flat_Obj_Buffers () : FlatObjectVB ( 0 ), InstanceVBs ( 0 ), IndexBs ( 0 ), RenderTexture (0)
	 {
		FlatObjectVB = nullptr;
		InstanceVBs  = nullptr;
		IndexBs = nullptr;
		RenderTexture = nullptr;
		ThisBufferIndex = -1;
	};
			 
	~Flat_Obj_Buffers ()
	{
		RCUBE_DELETE ( FlatObjectVB )
		RCUBE_DELETE ( InstanceVBs )
		RCUBE_DELETE ( IndexBs )
	}

	void SetVertexBuffer ()
	{
//		FlatObjectVB.

	}
	VertexBuffer <Vertex_FlatObject>* FlatObjectVB;
	VertexBuffer <PositionType>* InstanceVBs;							// Instance buffer 
	IndexBuffer < Index_Type >* IndexBs;
	DXTextureSRV* RenderTexture;
	int ThisBufferIndex;

};


class _3D_Obj_Buffers
{
public:
	_3D_Obj_Buffers () : Vertexes ( 0 ), Indexes ( 0 )
	{
		Vertexes = nullptr;
		Indexes = nullptr;
		ThisBufferIndex = -1;
};

	~_3D_Obj_Buffers ()
	{
		RCUBE_DELETE ( Vertexes )
		RCUBE_DELETE ( Indexes )
	}

	void SetVertexBuffer ()
	{
		//		FlatObjectVB.

	}
	VertexBuffer <Vertex_Model3D>* Vertexes;
	IndexBuffer < Index_Type >* Indexes;
	int ThisBufferIndex;

};
#endif //RCube_DX11



#ifdef RCube_DX12
#include <d3d12.h>

#endif //RCube_DX12

#endif _DX11BUFFERS_H_
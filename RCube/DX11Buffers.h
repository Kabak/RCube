// Copyright 2010 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.

#pragma once
#include "stdafx.h"
#include <vector>

#ifdef RCube_DX11
#include <d3d11.h>

// + BUFFER
template <typename T>
class Buffer
{
public:
	Buffer ( ID3D11Device* d3dDevice, int elements,
		UINT bindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
		bool dynamic = false , UINT MiscFlags, UINT Stride );

   ~Buffer ();
   
   ID3D11Buffer* GetBuffer () { return mBuffer; }
   ID3D11UnorderedAccessView* GetUnorderedAccess () { return mUnorderedAccess; }
   ID3D11ShaderResourceView* GetShaderResource () { return mShaderResource; }

   inline T* MapDiscard ( ID3D11DeviceContext* d3dDeviceContext );
   void Unmap ( ID3D11DeviceContext* d3dDeviceContext );

private:
	int mElements;
	ID3D11Buffer* mBuffer;
	ID3D11ShaderResourceView* mShaderResource;
	ID3D11UnorderedAccessView* mUnorderedAccess;
};

template <typename T>
Buffer<T>::Buffer ( ID3D11Device* d3dDevice, int elements,
	UINT bindFlags, bool dynamic, UINT MiscFlags, UINT Stride )
	: mElements ( elements ), mShaderResource ( 0 ), mUnorderedAccess ( 0 )
{
	CD3D11_BUFFER_DESC desc ( sizeof ( T ) * elements, bindFlags,
		dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT,
		dynamic ? D3D11_CPU_ACCESS_WRITE : 0,
		MiscFlags,
		Stride,
		sizeof ( T ) );

	HRESULT res = d3dDevice->CreateBuffer ( &desc, 0, &mBuffer );
	assert ( res == S_OK );
	if (bindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		res = d3dDevice->CreateUnorderedAccessView ( mBuffer, 0, &mUnorderedAccess );
		assert ( res == S_OK );
	}

	if (bindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		res = d3dDevice->CreateShaderResourceView ( mBuffer, 0, &mShaderResource );
		assert ( res == S_OK );
	}
}


template <typename T>
Buffer<T>::~Buffer ()
{
	RCUBE_RELEASE ( mUnorderedAccess );
	RCUBE_RELEASE ( mShaderResource );
	RCUBE_RELEASE ( mBuffer );
}


template <typename T>
T* Buffer<T>::MapDiscard ( ID3D11DeviceContext* d3dDeviceContext )
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	d3dDeviceContext->Map ( mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
	return static_cast<T*>(mappedResource.pData);
}


template <typename T>
void Buffer<T>::Unmap ( ID3D11DeviceContext* d3dDeviceContext )
{
	d3dDeviceContext->Unmap ( mBuffer, 0 );
}
// - BUFFER

// + VERTEX BUFFER
template <typename T>
class VertexBuffer
{
public:
	VertexBuffer ( ID3D11Device* d3dDevice, UINT Usage, int elements, void* Vertexes = nullptr );
   ~VertexBuffer ();
	
   void UpdateBuffer ( ID3D11DeviceContext* d3dDeviceContext, void* Vertexes );
   inline T* MapDiscard ( ID3D11DeviceContext* d3dDeviceContext );
   void Unmap ( ID3D11DeviceContext* d3dDeviceContext );

private:
	int mElements;
	ID3D11Buffer* mBuffer;

};

template <typename T>
VertexBuffer<T>::VertexBuffer ( ID3D11Device* d3dDevice, UINT Usage, int elements, void* Vertexes )
{
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory ( &vertexBufferData, sizeof ( vertexBufferData ) );

	desc.ByteWidth = sizeof ( T ) * elements;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.Usage = Usage;

	if (Usage & D3D11_USAGE_DEFAULT || Usage & D3D11_USAGE_IMMUTABLE)
	{
		desc.CPUAccessFlags = 0;

		vertexBufferData.pSysMem = static_cast<T*>Vertexes;

	}
	else if ( Usage & D3D11_USAGE_DYNAMIC )
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	else if ( Usage & D3D11_USAGE_STAGING )
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	HRESULT res = d3dDevice->CreateBuffer ( &desc, &vertexBufferData, &mBuffer );
	assert ( res == S_OK );
}

template <typename T>
void VertexBuffer<T>::UpdateBuffer ( ID3D11DeviceContext* d3dDeviceContext, void* Vertexes )
{
	d3dDeviceContext->UpdateSubresource ( mBuffer, 0, 0, Vertexes, 0 );
}


template <typename T>
T* VertexBuffer<T>::MapDiscard ( ID3D11DeviceContext* d3dDeviceContext )
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	d3dDeviceContext->Map ( mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
	return static_cast<T*>(mappedResource.pData);
}


template <typename T>
void VertexBuffer<T>::Unmap ( ID3D11DeviceContext* d3dDeviceContext )
{
	d3dDeviceContext->Unmap ( mBuffer, 0 );
}


template <typename T>
VertexBuffer<T>::~VertexBuffer ()
{
	RCUBE_RELEASE ( mBuffer );
}







// - VERTEX BUFFER

#endif //RCube_DX11



#ifdef RCube_DX12
#include <d3d12.h>

#endif //RCube_DX12
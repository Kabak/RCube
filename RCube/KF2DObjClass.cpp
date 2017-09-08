#include "stdafx.h"
#include "KF2DObjClass.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++ базовый класс для рисования всех обьектов в 2D
KF2DObjClass::KF2DObjClass()
{

	g_IndexBuffer	= nullptr;
	g_VertexBuffer	= nullptr;
	m_layout		= nullptr;
	m_sampleState	= nullptr;
			D3DGC	= nullptr;
}


KF2DObjClass::~KF2DObjClass()
{

	RCUBE_RELEASE ( g_VertexBuffer );
	RCUBE_RELEASE ( g_IndexBuffer );
	RCUBE_RELEASE ( m_layout );
	RCUBE_RELEASE ( m_sampleState );

}


HRESULT KF2DObjClass::Init( 
	D3DGlobalContext* _D3DGC,
	InterfaceVertexType* Vertexes, 
	unsigned long* Indeces, 
	ID3D11ShaderResourceView * texture, 
	int VertexCount, 
	int IndexCount,
	ID3D10Blob * WorkBlob
	)
{
					
	HRESULT result = S_OK;

			  D3DGC = _D3DGC;
	   	   m_layout = nullptr;

		  VertexNum = VertexCount;
		   IndexNum = IndexCount;

	
	// создание буферов
	D3D11_BUFFER_DESC bd ;
	D3D11_SUBRESOURCE_DATA InitData;

	// Set up the description of the static vertex buffer.
            bd.Usage = D3D11_USAGE_DEFAULT;
            bd.ByteWidth = sizeof(InterfaceVertexType) * VertexCount;
            bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            bd.CPUAccessFlags = 0;
            bd.MiscFlags = 0;
	        bd.StructureByteStride = 0;
			
	// Give the subresource structure a pointer to the vertex data.
            InitData.pSysMem = Vertexes;
	        InitData.SysMemPitch = 0;
	        InitData.SysMemSlicePitch = 0;
			result = D3DGC->D11_device->CreateBuffer( &bd, &InitData, &g_VertexBuffer );
            if( FAILED( result ) ){
				MessageBox(D3DGC->hwnd, L"KFObjClass. ошибка в создании вертексного буфера.", L"Error", MB_OK);
				return result;
			}

	UINT stride = sizeof( InterfaceVertexType );
    UINT offset = 0;
	
    // Create index buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( unsigned int ) * IndexCount;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
    InitData.pSysMem = Indeces;
	result = D3DGC->D11_device->CreateBuffer( &bd, &InitData, &g_IndexBuffer );

    if( FAILED( result ) ){
		MessageBox( D3DGC->hwnd, L"KFObjClass. ошибка в создании индексного буфера.", L"Error", MB_OK);
        return result;
	}

	Texture = texture;
	
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = D3DGC->D11_device->CreateInputLayout(polygonLayout, numElements, WorkBlob->GetBufferPointer(), WorkBlob->GetBufferSize(),
					   &m_layout);
	if(FAILED(result))
	{
		MessageBox( D3DGC->hwnd, L"KFObjClass. ошибка в создании лайаута.", L"Error", MB_OK);
		return result;
	}

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // D3D11_FILTER_COMPARISON_ANISOTROPIC ;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = D3DGC->D11_device->CreateSamplerState(&samplerDesc, &m_sampleState);

	if(FAILED(result))
	{
		MessageBox( D3DGC->hwnd, L"KFObjClass. ошибка в создании самплер стэйта.", L"Error", MB_OK);
		return result;
	}
	
	return result;
}


void KF2DObjClass::Render(){

	unsigned int stride;
	unsigned int offset;
	
	// Set vertex buffer stride and offset.
	stride = sizeof(InterfaceVertexType);
	offset = 0;

	D3DGC->D11_deviceContext->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	D3DGC->D11_deviceContext->IASetIndexBuffer(g_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

//	D3DGC->D11_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DGC->D11_deviceContext->PSSetShaderResources(0, 1, &Texture);

	D3DGC->D11_deviceContext->IASetInputLayout(m_layout);

	D3DGC->D11_deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	D3DGC->D11_deviceContext->DrawIndexed(IndexNum, 0, 0);

}


void KF2DObjClass::SetObjectTexture(ID3D11ShaderResourceView* texture)
{
	Texture = texture ;
}


void KF2DObjClass::SetVertexBuffer(InterfaceVertexType* Vertexes )
{
	D3DGC->D11_deviceContext->UpdateSubresource( g_VertexBuffer, NULL, NULL, Vertexes, NULL, NULL );
}

ID3D11ShaderResourceView* KF2DObjClass::GetTexture(){

	return Texture ;

}
//-------------------------------------------------------------------------------------- базовый класс для рисования всех обьектов в 2D
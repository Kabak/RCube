#include "stdafx.h"
#include "KFCubeMappingClass.h"
//#include <DDSTextureLoader.cpp>

D3D11_INPUT_ELEMENT_DESC Mlayout[] =
{
	//{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // позиция вершины
	//{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },  // нормаль для освещения
	//{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },   // координаты в текстуре наложения

	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	//{ "NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
UINT MapnumElements = ARRAYSIZE(Mlayout);

KFCubeMapping::KFCubeMapping(){
	vertLayout = nullptr;
	d3d11DevCon = nullptr;
	d3d11Device = nullptr;

	sphereIndexBuffer = nullptr;
	sphereVertBuffer = nullptr;

	cbPerObjectBuffer = nullptr;
	smrv = nullptr;
	CubesTexSamplerState = nullptr;


	RSCullNone = nullptr;
	DSLessEqual = nullptr;

}

HRESULT KFCubeMapping::Init(ID3D11DeviceContext * ActiveDeviceContext , ID3D11Device * ActiveDevice , ID3D11ShaderResourceView * Texture,
	ID3D10Blob * Blob){


	smrv = Texture;
	d3d11DevCon = ActiveDeviceContext ;
	d3d11Device = ActiveDevice ;

	int LatLines = 10 ;
	int LongLines = 10 ;

	// создание беферов сферы+++++++++++++++++++++++++
	NumSphereVertices = ((LatLines-2) * LongLines) + 2;
	NumSphereFaces  = ((LatLines-3)*(LongLines)*2) + (LongLines*2);

	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	std::vector<Vertex> vertices(NumSphereVertices);
	
	XMVECTOR currVertPos = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	vertices[0].pos.x = 0.0f;
	vertices[0].pos.y = 0.0f;
	vertices[0].pos.z = 1.0f;

	for(int i = 0; i < LatLines-2; ++i)
	{
		spherePitch = float((i+1) * (3.14/(LatLines-1)));
		Rotationx = XMMatrixRotationX(spherePitch);
		for(int j = 0; j < LongLines; ++j)
		{
			sphereYaw = float(j * (6.28/(LongLines)));
			Rotationy = XMMatrixRotationZ(sphereYaw);
			currVertPos = XMVector3TransformNormal( XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx * Rotationy) );	
			currVertPos = XMVector3Normalize( currVertPos );
			vertices[i*LongLines+j+1].pos.x = XMVectorGetX(currVertPos);
			vertices[i*LongLines+j+1].pos.y = XMVectorGetY(currVertPos);
			vertices[i*LongLines+j+1].pos.z = XMVectorGetZ(currVertPos);
		}
	}

	vertices[NumSphereVertices-1].pos.x =  0.0f;
	vertices[NumSphereVertices-1].pos.y =  0.0f;
	vertices[NumSphereVertices-1].pos.z = -1.0f;

	
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof( Vertex ) * NumSphereVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData; 

	ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
	vertexBufferData.pSysMem = &vertices[0];
	HRESULT hr = d3d11Device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &sphereVertBuffer);

	
	std::vector<DWORD> indices(NumSphereFaces * 3);

	int k = 0;
	for(int l = 0; l < LongLines-1; ++l)
	{
		indices[k] = 0;
		indices[k+1] = l+1;
		indices[k+2] = l+2;
		k += 3;
	}

	indices[k] = 0;
	indices[k+1] = LongLines;
	indices[k+2] = 1;
	k += 3;

	for(int i = 0; i < LatLines-3; ++i)
	{
		for(int j = 0; j < LongLines-1; ++j)
		{
			indices[k]   = i*LongLines+j+1;
			indices[k+1] = i*LongLines+j+2;
			indices[k+2] = (i+1)*LongLines+j+1;

			indices[k+3] = (i+1)*LongLines+j+1;
			indices[k+4] = i*LongLines+j+2;
			indices[k+5] = (i+1)*LongLines+j+2;

			k += 6; // next quad
		}

		indices[k]   = (i*LongLines)+LongLines;
		indices[k+1] = (i*LongLines)+1;
		indices[k+2] = ((i+1)*LongLines)+LongLines;

		indices[k+3] = ((i+1)*LongLines)+LongLines;
		indices[k+4] = (i*LongLines)+1;
		indices[k+5] = ((i+1)*LongLines)+1;

		k += 6;
	}

	for(int l = 0; l < LongLines-1; ++l)
	{
		indices[k] = NumSphereVertices-1;
		indices[k+1] = (NumSphereVertices-1)-(l+1);
		indices[k+2] = (NumSphereVertices-1)-(l+2);
		k += 3;
	}

	indices[k] = NumSphereVertices-1;
	indices[k+1] = (NumSphereVertices-1)-LongLines;
	indices[k+2] = NumSphereVertices-2;
	
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * NumSphereFaces * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = &indices[0];
	d3d11Device->CreateBuffer(&indexBufferDesc, &iinitData, &sphereIndexBuffer);
	// создание беферов сферы-------------------------------------------------

	// создание буфферов 
		D3D11_BUFFER_DESC cbbd;	
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = d3d11Device->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);
	

		//Tell D3D we will be loading a cube texture
// +++++++++++++++++++++++++++  Замена кода для новой версии DirectX  ++++++++++++++++++++++++++++++++++++++++++++
//	D3DX11_IMAGE_LOAD_INFO loadSMInfo;
//	loadSMInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		//Load the texture
//	ID3D11Texture2D* SMTexture = 0;
	// Describe the Sample State
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof(sampDesc) );
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;    
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	
	//Create the Sample State
	hr = d3d11Device->CreateSamplerState( &sampDesc, &CubesTexSamplerState );



	//Create the Input Layout
	hr = d3d11Device->CreateInputLayout(Mlayout, MapnumElements, Blob->GetBufferPointer(),
		Blob->GetBufferSize(), &vertLayout);


	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));

	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.FrontCounterClockwise = true;

	cmdesc.CullMode = D3D11_CULL_NONE;
	hr = d3d11Device->CreateRasterizerState(&cmdesc, &RSCullNone);

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	d3d11Device->CreateDepthStencilState(&dssDesc, &DSLessEqual);
	
	return S_OK ;

}

void KFCubeMapping::Render(const XMMATRIX& WorldViewProj ){

	UINT stride = sizeof( Vertex );
	UINT offset = 0;
	d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//Set the spheres index buffer
	d3d11DevCon->IASetIndexBuffer( sphereIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//Set the spheres vertex buffer
	d3d11DevCon->IASetVertexBuffers( 0, 1, &sphereVertBuffer, &stride, &offset );

	cbPerObj.WVP = XMMatrixTranspose( WorldViewProj );
	d3d11DevCon->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
	d3d11DevCon->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );

	//Send our skymap resource view to pixel shader
	d3d11DevCon->PSSetShaderResources( 0, 1, &smrv );
	d3d11DevCon->PSSetSamplers( 0, 1, &CubesTexSamplerState );

	//Set the new VS and PS shaders
	d3d11DevCon->IASetInputLayout(vertLayout);
	//Set the new depth/stencil and RS states
	d3d11DevCon->OMSetDepthStencilState(DSLessEqual, 0);
	d3d11DevCon->RSSetState(RSCullNone);
	d3d11DevCon->DrawIndexed( NumSphereFaces * 3, 0, 0 );

	d3d11DevCon->OMSetDepthStencilState(NULL, 0);

}

KFCubeMapping::~KFCubeMapping()
{
	RCUBE_RELEASE ( vertLayout );
	RCUBE_RELEASE ( sphereIndexBuffer );
	RCUBE_RELEASE ( sphereVertBuffer );
	RCUBE_RELEASE ( cbPerObjectBuffer );
	RCUBE_RELEASE ( CubesTexSamplerState );
	RCUBE_RELEASE ( RSCullNone );
	RCUBE_RELEASE ( DSLessEqual );
}
#include "stdafx.h"
#include "CubeMappingClass.h"


CubeMapping::CubeMapping(){

	d3d11DevCon = nullptr;
	d3d11Device = nullptr;

	sphereIndexBuffer = nullptr;
	sphereVertBuffer = nullptr;

	smrv = nullptr;
	DSLessEqual = nullptr;

}

HRESULT CubeMapping::Init( D3DGlobalContext *D3DGC, ID3D11ShaderResourceView * Texture){


	smrv = Texture;

	Local_D3DGC = D3DGC;

	int LatLines = 10 ;
	int LongLines = 10 ;

	// סמחהאםטו בופונמג ספונû+++++++++++++++++++++++++
	NumSphereVertices = ((LatLines-2) * LongLines) + 2;
	NumSphereFaces  = ((LatLines-3)*(LongLines)*2) + (LongLines*2);

	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	std::vector<Vertex_FlatObject> vertices(NumSphereVertices);
	
	RCube_VecFloat34 currVertPos;
		currVertPos.Vec = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	vertices[0].Position = currVertPos.Fl3;

	for(int i = 0; i < LatLines-2; ++i)
	{
		spherePitch = float((i+1) * (3.14/(LatLines-1)));
		Rotationx = XMMatrixRotationX(spherePitch);
		for(int j = 0; j < LongLines; ++j)
		{
			sphereYaw = float(j * (6.28/(LongLines)));
			Rotationy = XMMatrixRotationZ(sphereYaw);
			currVertPos.Vec = XMVector3TransformNormal( XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx * Rotationy) );
			currVertPos.Vec = XMVector3Normalize( currVertPos.Vec );

			vertices[i*LongLines + j + 1].Position = currVertPos.Fl3;
		}
	}

	vertices[NumSphereVertices - 1].Position = { 0.0f, 0.0f, -1.0f };
	
	
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof( Vertex_FlatObject ) * NumSphereVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData; 

	ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
	vertexBufferData.pSysMem = &vertices[0];
	HRESULT hr = Local_D3DGC->D11_device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &sphereVertBuffer);

	
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
	Local_D3DGC->D11_device->CreateBuffer(&indexBufferDesc, &iinitData, &sphereIndexBuffer);
	// סמחהאםטו בופונמג ספונû-------------------------------------------------


	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	Local_D3DGC->D11_device->CreateDepthStencilState(&dssDesc, &DSLessEqual);
	
	return S_OK ;

}

void CubeMapping::Render(){

	UINT stride = sizeof( Vertex_FlatObject );
	UINT offset = 0;
	Local_D3DGC->D11_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Set the spheres index buffer
	Local_D3DGC->D11_deviceContext->IASetIndexBuffer( sphereIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//Set the spheres vertex buffer
	Local_D3DGC->D11_deviceContext->IASetVertexBuffers( 0, 1, &sphereVertBuffer, &stride, &offset );

	//Send our skymap resource view to pixel shader
	Local_D3DGC->D11_deviceContext->PSSetShaderResources( 0, 1, &smrv );
//	Local_D3DGC->D11_deviceContext->PSSetSamplers( 0, 1, &CubesTexSamplerState );

	//Set the new depth/stencil and RS states
	Local_D3DGC->D11_deviceContext->OMSetDepthStencilState(DSLessEqual, 0);
	Local_D3DGC->D11_deviceContext->DrawIndexed( NumSphereFaces * 3, 0, 0 );

//	d3d11DevCon->OMSetDepthStencilState(NULL, 0);

}

CubeMapping::~CubeMapping()
{
	RCUBE_RELEASE ( sphereIndexBuffer );
	RCUBE_RELEASE ( sphereVertBuffer );
	RCUBE_RELEASE ( DSLessEqual );
}
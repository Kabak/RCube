#include "stdafx.h"
////////////////////////////////////////////////////////////////////////////////
// Filename: fontshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "fontshaderclass.h"


FontShaderClass::FontShaderClass()
{
	//m_vertexShader = nullptr;
	// m_pixelShader = nullptr;
		  m_layout = nullptr;
	 m_sampleState = nullptr;
	 m_pixelBuffer = nullptr;
		  OldValue = XMVECTOR{0.0f, 0.0f, 0.0f, 0.0f};
}


FontShaderClass::FontShaderClass(const FontShaderClass& other)
{
}


FontShaderClass::~FontShaderClass()
{
}


bool FontShaderClass::Initialize(ID3D11Device* device, HWND hwnd , ID3D10Blob* Blob)
{
	HRESULT result;

	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC pixelBufferDesc;


/*
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the FontShaderClass and in the shader.
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
*/
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2] =
	{
		{ "POSITION" , 0 , DXGI_FORMAT_R32G32B32_FLOAT , 0 , 0							  , D3D11_INPUT_PER_VERTEX_DATA , 0 },
		{ "TEXCOORD" , 0 , DXGI_FORMAT_R32G32_FLOAT    , 0 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA , 0 },
	};

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, Blob->GetBufferPointer(),
		Blob->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		Shutdown();
		return false;
	}

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; //D3D11_FILTER_COMPARISON_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER; //D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not Initialize FontShaderClass CreateSamplerState.", L"Error", MB_OK);
		Shutdown();
		return false;
	}

	// Here we setup the new pixel color constant buffer that will allow this class to set the pixel color in the pixel shader.

	// Setup the description of the dynamic pixel constant buffer that is in the pixel shader.
	pixelBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	pixelBufferDesc.ByteWidth = sizeof(PixelBufferType);
	pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelBufferDesc.CPUAccessFlags = 0;
	pixelBufferDesc.MiscFlags = 0;
	pixelBufferDesc.StructureByteStride = 0;

	// Create the pixel constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	result = device->CreateBuffer(&pixelBufferDesc, NULL, &m_pixelBuffer);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not Initialize FontShaderClass CreateBuffer.", L"Error", MB_OK);
		Shutdown();
		return false;
	}

	return true;
}

// Shutdown calls ShutdownShader which releases the font shader related pointers and data.

void FontShaderClass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();
}


void FontShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount,  ID3D11ShaderResourceView* texture
	, XMVECTOR& pixelColor)
{
	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	// Обновляем константбуфер, только если нужно записать новое значение цвета
	if (!XMVector3Equal(OldValue, pixelColor))
	{
		PixelBufferType dataPtr2;
		dataPtr2.pixelColor = pixelColor;

		deviceContext->UpdateSubresource(m_pixelBuffer, NULL, NULL, &dataPtr2, NULL, NULL);
		deviceContext->PSSetConstantBuffers(13, 1, &m_pixelBuffer);
		OldValue = pixelColor;
	}

	deviceContext->DrawIndexed(indexCount, 0, 0);
}


// The ShutdownShader function releases all the shader related data.

void FontShaderClass::ShutdownShader()
{
	RCUBE_RELEASE(m_pixelBuffer);
	RCUBE_RELEASE(m_sampleState);
	RCUBE_RELEASE(m_layout);
}

// OutputShaderErrorMessage writes any shader compilation errors to a text file that can be reviewed in the event of a failure in compilation.

void FontShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = (ULONG)errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for(i=0; i<bufferSize; ++i)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}


void FontShaderClass::SetFontLayoutSempler(ID3D11DeviceContext* deviceContext)
{
	deviceContext->IASetInputLayout(m_layout);
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);
}
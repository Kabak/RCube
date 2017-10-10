#include "stdafx.h"
////////////////////////////////////////////////////////////////////////////////
// Filename: fontshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "fontshaderclass.h"


FontShaderClass::FontShaderClass()
{
	 m_pixelBuffer = nullptr;
		  OldValue = XMVECTOR{0.0f, 0.0f, 0.0f, 0.0f};
}


FontShaderClass::FontShaderClass(const FontShaderClass& other)
{
}


FontShaderClass::~FontShaderClass()
{
}


bool FontShaderClass::Initialize( D3DGlobalContext* D3DGC )
{
	HRESULT result;

	D3D11_BUFFER_DESC pixelBufferDesc;

	// Here we setup the new pixel color constant buffer that will allow this class to set the pixel color in the pixel shader.

	// Setup the description of the dynamic pixel constant buffer that is in the pixel shader.
	pixelBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	pixelBufferDesc.ByteWidth = sizeof(PixelBufferType);
	pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelBufferDesc.CPUAccessFlags = 0;
	pixelBufferDesc.MiscFlags = 0;
	pixelBufferDesc.StructureByteStride = 0;

	// Create the pixel constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	result = D3DGC->D11_device->CreateBuffer(&pixelBufferDesc, NULL, &m_pixelBuffer);
	if (FAILED(result))
	{
		MessageBox( D3DGC->hwnd, L"Could not Initialize FontShaderClass CreateBuffer.", L"Error", MB_OK);
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

}
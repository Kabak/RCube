#pragma once
#include "stdafx.h"

#include "RenderClass.h"

RenderClass::RenderClass ( D3DGlobalContext* GlobalContext, ResourceManager * _ResManager, D3DClass* m_D3D, FrustumClass* _Frustum )
{
	ResManager  = _ResManager;
	Local_D3DGC =  GlobalContext;
	Local_D3D   =  m_D3D;
	Local_Frustum = _Frustum;

	DWriteFactory		= Local_D3DGC->DWriteFactory;
	D2DFactory			= Local_D3DGC->D2DFactory;
	D2DRenderTarget		= Local_D3DGC->D2DRenderTarget;
	keyedMutex11		= Local_D3DGC->keyedMutex11;
	keyedMutex10		= Local_D3DGC->keyedMutex10;
	Created_textureBuf	= Local_D3DGC->sharedTex11_MAPED;
	sharedTex11			= Local_D3DGC->sharedTex11;
	Width				= Local_D3DGC->sharedTex11_Width;
	Height				= Local_D3DGC->sharedTex11_Height;
	d2dTexture			= Local_D3DGC->sharedTex11_SRV;

	ZeroMemory ( &DSD2, sizeof ( DSD2 ) );
	DSD2.Flags = 0;
	DSD2.Format = DXGI_FORMAT_D32_FLOAT;
	if ( Local_D3DGC->MSAAQualityCount > 1 )
	{
		DSD2.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
		DSD2.Texture2DMSArray.ArraySize = 1;
		DSD2.Texture2DMSArray.FirstArraySlice = 0;
	}
	else
	{
		DSD2.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		DSD2.Texture2DArray.MipSlice = 0;
		DSD2.Texture2DArray.ArraySize = 1;
		DSD2.Texture2DArray.FirstArraySlice = 0;
	}

	Up = XMVectorSet ( 0.0f, 1.0f, 0.0f, 0.0f );
	LightPosition.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };
	LightTarget.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };

}


RenderClass::~RenderClass ()
{

}


void RenderClass::RenderMenu (int Index)
{
	size_t c = ResManager->Menus.size ();
	
	Menu *TempMenu = ResManager->Menus[Index];

	Flat_Obj_Buffers* FlatObject;

	if (Index < c)
	{
		// Если есть фон, рисуем его
		if (TempMenu->Background)
		{
			FlatObject = ResManager->Get_Flat_ObjectBuffers_ByIndex ( TempMenu->Background->Buffers->ThisBufferIndex );
			RenderFlatObject ( FlatObject );
		}

		// Если есть анимация в меню, то рисуем её
		if (TempMenu->Animation != nullptr && TempMenu->Animation->Enabled )
		{
			FlatObject = ResManager->Get_Flat_ObjectBuffers_ByIndex ( TempMenu->Animation->AnimeTexture->Buffers->ThisBufferIndex );
			// Устанавливаем специфический шейдер для анимации
//			ResManager->SetActiveShadersInProgramm ( TempMenu->Animation->ShaderForDraw );
			RenderFlatObject ( FlatObject );
			// Возвращаем шейдер для отрисовки остальных элементов меню
//			ResManager->SetActiveShadersInProgramm ( TempMenu->Animation->ShaderReturnTo );
		}
		// Buttons
		int c = 0;
		while ( c < TempMenu->g_NumOfButtons )
		{
//			KFButton* Button = TempMenu->Buttons[c];

			FlatObject = ResManager->Get_Flat_ObjectBuffers_ByIndex ( TempMenu->Buttons[c]->Body->Buffers->ThisBufferIndex );
			RenderFlatObject ( FlatObject );

			++c;
		}
		// Scrollbars
		c = 0;
		while (c < TempMenu->g_NumOfScrollBars)
		{

			ScrollBarClass* ScrollBar = TempMenu->ScrollBars[c];

			FlatObject = ResManager->Get_Flat_ObjectBuffers_ByIndex ( ScrollBar->Body->Buffers->ThisBufferIndex );
			RenderFlatObject ( FlatObject );
			// исуем три кнопки от скролбара
			if ( ScrollBar->MinMaxButtonsShow)
			{
				FlatObject = ResManager->Get_Flat_ObjectBuffers_ByIndex ( ScrollBar->MinButton->Buffers->ThisBufferIndex );
				RenderFlatObject ( FlatObject );
				FlatObject = ResManager->Get_Flat_ObjectBuffers_ByIndex ( ScrollBar->MaxButton->Buffers->ThisBufferIndex );
				RenderFlatObject ( FlatObject );
			}

			FlatObject = ResManager->Get_Flat_ObjectBuffers_ByIndex ( ScrollBar->Traveller->Buffers->ThisBufferIndex );
			RenderFlatObject ( FlatObject );
			++c;
		}
		// StringLists
		c = 0;
		while (c < TempMenu->g_NumOfStringsLists)
		{

			StringsListClass* Strings = TempMenu->StringsList[c];

			// Setting the temporary texture for drawing text strings
			Local_D3DGC->DX_deviceContext->ClearRenderTargetView ( Local_D3DGC->BackBuffer_ProxyTextureRTV, Local_D3DGC->ZeroColor );
			Local_D3DGC->DX_deviceContext->OMSetRenderTargets ( 1, &Local_D3DGC->BackBuffer_ProxyTextureRTV, NULL );
			// Рисуем строки в текстуру для скролинга
			RenderText ( Strings->SentencesIndex );

			// Сохраняем в файл для визуальной отладки
			//	Local_D3D->SaveTextureToPNG( Local_D3DGC->BackBuffer_ProxyTextureSRV );

			Local_D3DGC->DX_deviceContext->OMSetRenderTargets ( 1, &Local_D3DGC->BackBuffer_RTV, Local_D3DGC->m_depthStencilView );

			FlatObject = ResManager->Get_Flat_ObjectBuffers_ByIndex ( Strings->StringListObj->Buffers->ThisBufferIndex );
			RenderFlatObject ( FlatObject );
			++c;
		}
		// ColorPickers
		c = 0;
		while ( c < TempMenu->g_NumOfColorPickers )
		{

			ColorPickerClass* ColorPickers = TempMenu->ColorPickers[c];

//			Local_D3DGC->DX_deviceContext->OMSetRenderTargets ( 1, &Local_D3DGC->BackBuffer_ProxyTextureRTV, NULL );

			unsigned int stride;
			unsigned int offset;

			// Set vertex buffer stride and offset.
			stride = sizeof ( Vertex_FlatObject );
			offset = 0;
			// Render ColorPicker Texture
//			FlatObject = ResManager->Get_Flat_ObjectBuffers_ByIndex ( ColorPickers->ColorPicker->Buffers->ThisBufferIndex );
//			Local_D3DGC->DX_deviceContext->IASetVertexBuffers ( 0, 1, &FlatObject->FlatObjectVB->Buffer, &stride, &offset );
//			Local_D3DGC->DX_deviceContext->IASetIndexBuffer ( FlatObject->IndexBs->Buffer, DXGI_FORMAT_R32_UINT, 0 );
//			Local_D3DGC->DX_deviceContext->IASetInputLayout ( ResManager->Layouts[FLATOBJECT] );
//			Local_D3DGC->DX_deviceContext->DrawIndexed ( 6, 0, 0 );

//			Local_D3D->SaveTextureToPNG ( Local_D3DGC->BackBuffer_ProxyTextureSRV );

			// Render AlphaPicker Texture
//			FlatObject = ResManager->Get_Flat_ObjectBuffers_ByIndex ( ColorPickers->AlphaPicker->Buffers->ThisBufferIndex );
//			Local_D3DGC->DX_deviceContext->IASetVertexBuffers ( 0, 1, &FlatObject->FlatObjectVB->Buffer, &stride, &offset );
//			Local_D3DGC->DX_deviceContext->DrawIndexed ( 6, 0, 0 );

//			Local_D3D->SaveTextureToPNG( Local_D3DGC->BackBuffer_ProxyTextureSRV );

			// Render All components on the screen
//			Local_D3DGC->DX_deviceContext->OMSetRenderTargets ( 1, &Local_D3DGC->BackBuffer_RTV, Local_D3DGC->m_depthStencilView );

			FlatObject = ResManager->Get_Flat_ObjectBuffers_ByIndex ( ColorPickers->Panthon->Buffers->ThisBufferIndex );
			RenderFlatObject ( FlatObject );
			FlatObject = ResManager->Get_Flat_ObjectBuffers_ByIndex ( ColorPickers->ColorPicker->Buffers->ThisBufferIndex );
			RenderFlatObject ( FlatObject );
			FlatObject = ResManager->Get_Flat_ObjectBuffers_ByIndex ( ColorPickers->AlphaPicker->Buffers->ThisBufferIndex );
			RenderFlatObject ( FlatObject );
			++c;
		}
	}

}


void RenderClass::RenderScene ()
{
	int Objects_Amount = ( int ) ResManager->_3DModels.size ();
	for(int i = 0; i < Objects_Amount; ++i )
	{
		Render3D_Object ( i, false );
	}

//	Local_D3DGC->DX_deviceContext->RSSetState ( Local_D3DGC->WireFrameRasterizerState );


	Objects_Amount = ( int ) ResManager->Terrains.size ();
	for ( int i = 0; i < Objects_Amount; ++i )
	{
		Terrain* Terrain = ResManager->Terrains[i];

		if ( Terrain->ClusterRender )
			ResManager->FrustumTerrain ( Local_Frustum, Terrain);

		RenderTerrain ( i, false );
	}

//	Local_D3DGC->DX_deviceContext->RSSetState ( Local_D3DGC->DefaultRasterizerState );
}


void RenderClass::RenderFlatObject ( Flat_Obj_Buffers* FlatObject )
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof ( Vertex_FlatObject );
	offset = 0;

	Local_D3DGC->DX_deviceContext->IASetVertexBuffers ( 0, 1, &FlatObject->FlatObjectVB->Buffer, &stride, &offset );

	Local_D3DGC->DX_deviceContext->IASetIndexBuffer ( FlatObject->IndexBs->Buffer, DXGI_FORMAT_R32_UINT, 0 );

//	Local_D3DGC->DX_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Local_D3DGC->DX_deviceContext->PSSetShaderResources ( 0, 1, &FlatObject->RenderTexture );

	Local_D3DGC->DX_deviceContext->IASetInputLayout( ResManager->Layouts[FLATOBJECT] );

	Local_D3DGC->DX_deviceContext->DrawIndexed ( 6, 0, 0 );
}



UINT RenderClass::PackUNorm ( float bitmask, float value )
{
	value *= bitmask;
	return (unsigned int)ClampAndRound ( value, 0.0f, bitmask );
}


float RenderClass::ClampAndRound ( float value, float min, float max )
{
	if (value <= min)
	{
		return min;
	}
	if (value >= max)
	{
		return max;
	}
	return Round ( value );
}


float RenderClass::Round ( float Val )
{
	float Temp;
	// Почему-то нарушаются шрифты, если это убрать
	if (Val > 228.0f)
		Temp = Val;
	else
		Temp = Val > 0.0f ? Val + 0.5f : Val - 0.5f;
	return floor ( Temp );
}


float RenderClass::RoundToByte ( float Val )
{
	float Temp;

	Temp = Val > 0.0f ? Val + 0.5f : Val - 0.5f;

	return floor ( Temp );
}


UINT RenderClass::PackHelper ( float vectorX, float vectorY, float vectorZ, float vectorW )
{
	unsigned int  num = PackUNorm ( 255.0f, vectorX );
	unsigned int num2 = PackUNorm ( 255.0f, vectorY ) << 8;
	unsigned int num3 = PackUNorm ( 255.0f, vectorZ ) << 16;
	unsigned int num4 = PackUNorm ( 255.0f, vectorW ) << 24;
	return num | num2 | num3 | num4;
}


int RenderClass::CalkNoSpaceAmount (const WCHAR* String )
{
	int Result = 0;
	size_t size = wcslen ( String );

	for (size_t i = 0; i < size; ++i)
	{
		String[i] != ' ' ? ++Result : Result;
	}

	return Result;
}


void RenderClass::CorrectSymbolsDim ( int OutlineSize, int NewTextureWidth, UINT SymbolAmount, _Symbol_Dim* Symbols )
{
	double OnePixelLength = 1 / (double)(NewTextureWidth); // Берём ширину текстуры которая будет сохраняться в файл
	OnePixelLength *= 1000000000;
	OnePixelLength = floor ( OnePixelLength );
	OnePixelLength /= 1000000000;

	float OnePixL = (float)OnePixelLength;

	int Correction = OutlineSize / 2;

	for (UINT i = 0; i < SymbolAmount; ++i)
	{
		_Symbol_Dim *Dest = &Symbols[i];

		Dest->Start = (Dest->Start_Pixel_XPos - Correction) * OnePixL;
		Dest->End = (Dest->End_Pixel_XPos + Correction + 1) * OnePixL;

	}
}


void RenderClass::CalcFTextureDimention ( UINT FonColour )
{
	Local_D3DGC->DX_deviceContext->CopyResource ( Created_textureBuf, sharedTex11 );

	D3D11_MAPPED_SUBRESOURCE  mapResource;

	hr = Local_D3DGC->DX_deviceContext->Map ( Created_textureBuf, 0, D3D11_MAP_READ, NULL, &mapResource );
	if (hr == S_OK)
	{
/*
		union _block
		{
			byte  *data;
			UINT *udata;
		} block;


		block.data = new byte[amount];
*/
		int amount = mapResource.DepthPitch; // RowPitch * Height;
		int ui_amount = mapResource.DepthPitch / 4; // количество пикселей из 4 байт

		// инициализируем массив
		UINT* Pointer = (UINT*)mapResource.pData;
//		memcpy ( block.data, mapResource.pData, amount );

		int Columns = ui_amount / Height;
		int x = 0;
		int y = 0;
		Top = 0;
		Right = 0;
		Bottom = 0;
		Left = 0;

		bool FirstPoint = true;

		for (int i = 0; i < ui_amount; ++i)
		{
			// ищем цвет отличный от цвета 0.0f - это и есть точка от символа
			if (Pointer[i] != 0 && Pointer[i] != FonColour)
			{
				if (FirstPoint == true)
				{
					FirstPoint = false;
					Top = y;
					Left = x;
				}

				if (x < Left)
				{
					Left = x;
				}
				if (x > Right)
				{
					Right = x + 1;
				}
				if (y > Bottom)
				{
					Bottom = y + 1;
				}
			}
			++x;
			// счётчик строк и столбцов пикселей в текстуре
			// изменяем координаты следующего пикселя в текстуре
			if (x == Columns)
			{
				++y;
				x = 0;
			}
		}
		FTextureWidth = ++Right; // Почему-то нужно прибавить 1 ( у буквы ё обрезался хвостик )
		FTextureHeight = ++Bottom - Top;
/*
	   Image img;
		img.width = FTextureWidth;
		img.height = FTextureHeight;
		img.format = DXGI_FORMAT_R8G8B8A8_UNORM;
		img.rowPitch = mapResource.RowPitch;
		img.slicePitch = mapResource.DepthPitch;
		img.pixels = (UCHAR*)mapResource.pData; //block.data;
		hr = SaveToWICFile(img, WIC_FLAGS_NONE, GetWICCodec(WIC_CODEC_PNG), L"222_Render.png");
		if (FAILED(hr))
		{
		MessageBox(0, L"Could not save font.PNG file.", Error, MB_OK);
		}
*/

//		delete[] block.data;
		Local_D3DGC->DX_deviceContext->Unmap ( Created_textureBuf, 0 );
	}
}


bool RenderClass::Render_Font_OnTexture ( ID3D11ShaderResourceView* textureForDraw,
	WCHAR* FontNameFullPath,
	WCHAR* text,
	RCube_Font_Style*FOTData
)
{
	bool result = true; // если всё классно, функция 

	IDWriteFontFile						   *g_pFontFile = nullptr;
	IDWriteFontFace							  *FontFace = nullptr;
	ID2D1PathGeometry					 *pPathGeometry = nullptr;
	ID2D1GeometrySink							 *pSink = nullptr;
	UINT								   *pCodePoints = nullptr;
	UINT16								 *pGlyphIndices = nullptr;

	hr = DWriteFactory->CreateFontFileReference ( FontNameFullPath, NULL, &g_pFontFile );
	if (FAILED ( hr ))
	{
		MessageBox ( NULL, L"Create font file reference failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	IDWriteFontFile* fontFileArray[] = { g_pFontFile };

	hr = DWriteFactory->CreateFontFace ( FOTData->FontFaceType, 1, fontFileArray, 0, DWRITE_FONT_SIMULATIONS_NONE, &FontFace );
	if (FAILED ( hr ))
	{
		MessageBox ( NULL, L"Create font file face failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	UINT L = (UINT)wcslen ( text );

	pCodePoints = new UINT[L];
	pGlyphIndices = new UINT16[L];

	ZeroMemory ( pCodePoints, sizeof ( UINT ) * L );
	ZeroMemory ( pGlyphIndices, sizeof ( UINT16 ) * L );

	//	memcpy( pCodePoints, text, sizeof ( UINT ) * L );

	UINT *Dest = &pCodePoints[0];
	wchar_t *Source = &text[0];
	for (UINT i = 0; i < L; ++i)
	{
		//		pCodePoints[i] = text[i];
		*Dest = *Source;
		++Dest;
		++Source;
	}

	hr = FontFace->GetGlyphIndicesW ( pCodePoints, L, pGlyphIndices );
	if (FAILED ( hr ))
	{
		MessageBox ( NULL, L"Create GetGlyphIndices failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	hr = D2DFactory->CreatePathGeometry ( &pPathGeometry );
	if (FAILED ( hr ))
	{
		MessageBox ( NULL, L"Create CreatePathGeometry failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	// Write to the path geometry using the geometry sink.
	hr = pPathGeometry->Open ( &pSink );
	if (FAILED ( hr ))
	{
		MessageBox ( NULL, L"Open pPathGeometry failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	hr = FontFace->GetGlyphRunOutline (
		FOTData->FontSize,//( 20.0f / 72.0f )*96.0f, // 36.0f - размер шрифта
		pGlyphIndices,
		NULL,
		NULL,
		L,
		FALSE,
		FALSE,
		pSink );
	if (FAILED ( hr ))
	{
		MessageBox ( NULL, L"GetGlyphRunOutline failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	hr = pSink->Close ();
	if (FAILED ( hr ))
	{
		MessageBox ( NULL, L"pSink close failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	D2DRenderTarget->SetAntialiasMode ( FOTData->AliasingMode );

	//Release the D3D 11 Device
	keyedMutex11->ReleaseSync ( 0 );

	//Use D3D10.1 device
	keyedMutex10->AcquireSync ( 0, 0 );

	D2DRenderTarget->BeginDraw ();

	// First make sure the transformation is set to the identity transformation.
	//	D2DRenderTarget->SetTransform ( D2D1::IdentityMatrix () );

	// Clear the background of the window with a white color.
	//	D2DRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	D2DRenderTarget->Clear ( D2D1::ColorF ( 0.0f, 0.0f, 0.0f, 0.0f ) );

	// shift the text down
	D2DRenderTarget->SetTransform ( D2D1::Matrix3x2F::Translation ( 0.0f, 30.0f ) );

	if (FOTData->Outline)
	{
		// Draw text outline
		D2DRenderTarget->DrawGeometry ( pPathGeometry, FOTData->OutlineBrush, (float)FOTData->OutlineWidth );
	}

	// Draw text body
	D2DRenderTarget->FillGeometry ( pPathGeometry, FOTData->FillBrush );

	hr = D2DRenderTarget->EndDraw ();
	if (FAILED ( hr ))
	{
		MessageBox ( NULL, L"D2DRenderTarget->EndDraw() failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	//Release the D3D10.1 Device
	keyedMutex10->ReleaseSync ( 1 );

	//Use the D3D11 Device
	keyedMutex11->AcquireSync ( 1, 0 );

	int Res1 = ResManager->Create_Flat_Obj_Buffers ( NO_CPU_ACCESS_BUFFER, 4, 6, nullptr );
	int Res2 = ResManager->Create_Flat_Obj_Buffers ( NO_CPU_ACCESS_BUFFER, 4, 6, nullptr );

	ResManager->SetActiveShadersInProgramm ( ResManager->Temp_Font_StyleShaderIndex );

	// Наносим текстуру с текстом на нужною нам текстуру
	CopyTextTextureOnTexture ( textureForDraw, 
		ResManager->Get_Flat_ObjectBuffers_ByIndex ( Res1 ),
		ResManager->Get_Flat_ObjectBuffers_ByIndex ( Res2 ) );

	ResManager->Delete_Flat_ObjectBuffers ( Res2 );
	ResManager->Delete_Flat_ObjectBuffers ( Res1 );
	
	Local_D3D->D3DGC->DX_deviceContext->ClearRenderTargetView ( Local_D3D->D3DGC->BackBuffer_ProxyTextureRTV, Local_D3D->D3DGC->ZeroColor );

EXIT:

	RCUBE_ARR_DELETE ( pCodePoints );
	RCUBE_ARR_DELETE ( pGlyphIndices );

	RCUBE_RELEASE ( pPathGeometry );
	RCUBE_RELEASE ( pSink );
	RCUBE_RELEASE ( FontFace );
	RCUBE_RELEASE ( g_pFontFile );

	return result;

}


bool RenderClass::CopyTextTextureOnTexture ( ID3D11ShaderResourceView* textureForDraw,
	Flat_Obj_Buffers* SourceRes,
	Flat_Obj_Buffers* DestRes )
{
	// Задаём цвет фона и размеры текстуры по умолчанию для класса, чтобы правильно чистилась текстура, и можно было писать длинные строки - 640 символов
	// потому-что цвет пустоты 0.0f 
	UINT ClearColour = PackHelper ( 0.0f, 0.0f, 0.0f, 0.0f );

	ID3D11Texture2D* InputTexture;
	ID3D11Texture2D* CutTextureCopyForMap;
	ID3D11Texture2D* CutTexture;
	ID3D11Resource* MyRes;
	D3D11_TEXTURE2D_DESC* InputTextureDesc = new D3D11_TEXTURE2D_DESC;
	D3D11_TEXTURE2D_DESC* CutTextureDesc = new D3D11_TEXTURE2D_DESC;

	ID3D11ShaderResourceView* CutTexture_SRV;
	D3D11_TEXTURE2D_DESC* CutTextureCopyForMapDesc = new D3D11_TEXTURE2D_DESC;

	ZeroMemory ( CutTextureDesc, sizeof ( D3D11_TEXTURE2D_DESC ) );

	textureForDraw->GetResource ( &MyRes );
	MyRes->QueryInterface<ID3D11Texture2D> ( &InputTexture );
	InputTexture->GetDesc ( InputTextureDesc );
	// Получаем размеры текстуры с текстом
	CalcFTextureDimention ( ClearColour );

	/*
	#if defined( DEBUG ) || defined( _DEBUG )
	const char c_szName[] = "InputTexture";
	tempResource->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(c_szName) - 1, c_szName);
	#endif
	*/

	// Создаём текстуру по размерам и параметрам куска содержащего текст
	CutTextureDesc->Width = FTextureWidth;
	CutTextureDesc->Height = FTextureHeight;
	CutTextureDesc->Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	CutTextureDesc->MipLevels = 1;
	CutTextureDesc->ArraySize = 1;
	CutTextureDesc->SampleDesc.Count = 1;
	CutTextureDesc->Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_STAGING;
	CutTextureDesc->CPUAccessFlags = 0;//D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	CutTextureDesc->BindFlags = D3D11_BIND_SHADER_RESOURCE;
	hr = Local_D3DGC->DX_device->CreateTexture2D ( CutTextureDesc, NULL, &CutTexture );
	if (FAILED ( hr ))
	{
		MessageBox ( NULL, L"Could not initialize CutTexture.", Error, MB_OK );
		return false;
	}

	hr = Local_D3DGC->DX_device->CreateShaderResourceView ( CutTexture, NULL, &CutTexture_SRV );
	if (FAILED ( hr ))
	{
		MessageBox ( NULL, L"Could not initialize ShaderResourceView CutTexture.", Error, MB_OK );
		return false;
	}


	// Копируем описание входной текстуры для скорости заполнения буферы CutTextureCopyForMapDesc
	memcpy ( CutTextureCopyForMapDesc, CutTextureDesc, sizeof ( D3D11_TEXTURE2D_DESC ) );
	CutTextureCopyForMapDesc->Usage = D3D11_USAGE_STAGING;
	CutTextureCopyForMapDesc->CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	CutTextureCopyForMapDesc->BindFlags = 0;
	hr = Local_D3DGC->DX_device->CreateTexture2D ( CutTextureCopyForMapDesc, NULL, &CutTextureCopyForMap );
	if (FAILED ( hr ))
	{
		MessageBox ( NULL, L"Could not initialize CutTextureCopyForMap .", Error, MB_OK );
		return false;
	}


	D3D11_BOX* Box = new D3D11_BOX;
	ZeroMemory ( Box, sizeof ( D3D11_BOX ) );

	Box->top = Top;
	Box->left = Left;
	Box->bottom = Bottom;
	Box->right = Right;
	Box->front = 0;
	Box->back = 1;

	ID3D11Resource* tempResource;
	//  Получаем текстуру на которой мы рисовали текст. Для того, чтобы вырезать кусок содержащий текст
	d2dTexture->GetResource ( &tempResource );
	// Вырезаем кусок с текстом и рисекм им на тесктуре
	Local_D3DGC->DX_deviceContext->CopySubresourceRegion ( CutTexture, 0, 0, 0, 0, tempResource, 0, Box );
	// GetResource нужно делать Release
	tempResource->Release ();

	// Render Texture on Texture с помощью map
	// Переносим текстуру с текстом на нужную текстуру только если текстура с текстом меньше или равна
	// по размерам текстуры на которую хотим перенести. В противном слцчае текст не переносится вообще.
	if (InputTextureDesc->Width >= CutTextureDesc->Width && InputTextureDesc->Height >= CutTextureDesc->Height)
	{
		// создаём 2 прямоугольных объекта с нужными текстурами
		FlatObjectClass *Picture = new FlatObjectClass;
		FlatObjectClass *Text_Pixture = new FlatObjectClass;

		XMFLOAT4 SqObjPos = { 0.0f, 0.0f, float ( InputTextureDesc->Width ), float ( InputTextureDesc->Height ) };

		hr = Picture->Init ( Local_D3DGC->ScreenWidth, Local_D3DGC->ScreenHeight , SqObjPos,
			textureForDraw,
			NO_FLIP,
			SourceRes
		);
		if (FAILED ( hr ))
		{
			MessageBox ( 0, L"Picture->Init.", Error, MB_OK );
		}


		// Вычисляем смещение, чтобы текст был посередине текстуры
		float RealPosX = static_cast<float>(InputTextureDesc->Width - CutTextureDesc->Width) / 2;
		float RealPosY = static_cast<float>(InputTextureDesc->Height - CutTextureDesc->Height) / 2;

		SqObjPos = { RealPosX, RealPosY, static_cast<float>(CutTextureDesc->Width) , static_cast<float>(CutTextureDesc->Height) };

		hr = Text_Pixture->Init ( Local_D3DGC->ScreenWidth, Local_D3DGC->ScreenHeight , SqObjPos,
			CutTexture_SRV,
			NO_FLIP,
			DestRes
		);
		if (FAILED ( hr ))
		{
			MessageBox ( 0, L"Text_Pixture->Init.", Error, MB_OK );
		}

		// +  РИСУЕМ текстуру и поверху рисуем текст
		RenderFlatObject ( SourceRes );
		RenderFlatObject ( DestRes );
		// -  РИСУЕМ текстуру и поверху рисуем текст

		ID3D11Resource* tempResource;
		//	ID3D11Texture2D* temptexture;
		//	D3D11_TEXTURE2D_DESC* temptextureDesc = new D3D11_TEXTURE2D_DESC;
		//  Получаем текстуру на которой мы рисовали текст. Для того, чтобы вырезать кусок содержащий текст
		Local_D3DGC->BackBuffer_ProxyTextureSRV->GetResource ( &tempResource );
		//		tempResource->QueryInterface<ID3D11Texture2D>( &temptexture );
		//		temptexture->GetDesc( temptextureDesc );

		Box->top = 0;
		Box->left = 0;
		Box->bottom = InputTextureDesc->Height;
		Box->right = InputTextureDesc->Width;
		Box->front = 0;
		Box->back = 1;

		// Вырезаем кусок с текстом на текстуре
		// Проверяем включено ли MSAA сглаживание
		if (Local_D3DGC->EnableMSAA && Local_D3DGC->MSAAQualityCount > 1)
		{
			Local_D3DGC->DX_deviceContext->ResolveSubresource ( Local_D3DGC->BackBuffer_CopyResolveTexture, 0, tempResource, 0, DXGI_FORMAT_R8G8B8A8_UNORM );
			Local_D3DGC->DX_deviceContext->CopySubresourceRegion ( MyRes, 0, 0, 0, 0, Local_D3DGC->BackBuffer_CopyResolveTexture, 0, Box );
		}
		else
		{
			Local_D3DGC->DX_deviceContext->CopySubresourceRegion ( MyRes, 0, 0, 0, 0, tempResource, 0, Box );
		}

		// GetResource нужно делать Release
		tempResource->Release ();
		//		temptexture->Release();
		//		RCUBE_DELETE( temptextureDesc );

		RCUBE_DELETE ( Picture );
		RCUBE_DELETE ( Text_Pixture );

	}

	RCUBE_RELEASE ( CutTexture_SRV );
	RCUBE_RELEASE ( CutTextureCopyForMap );
	RCUBE_RELEASE ( InputTexture );
	RCUBE_RELEASE ( MyRes );
	RCUBE_RELEASE ( CutTexture );
	delete Box;
	delete CutTextureDesc;
	delete InputTextureDesc;
	delete CutTextureCopyForMapDesc;
	return true;
}


FontClass* RenderClass::Create_RCube_Font ( RCube_Font_Style* FOTData1, WCHAR* SymbolsString )
{
	bool SaveFont = false;		// Если нужно сохранить текстуру шрифта
	bool SaveFontData = false;	// Если нужно сохранить данные к текстуре шрифта

	UINT SymbolsNumber = CalkNoSpaceAmount ( SymbolsString );

	FontClass* NewFont = new FontClass ( SymbolsNumber, FOTData1->FontSize, SymbolsString );

//	ResManager->m_Font.push_back ( NewFont );	// Сохраняем шрифт в списке

	FOTData1->OutlineWidth > 10 ? FOTData1->OutlineWidth = 10 : FOTData1->OutlineWidth;
	FOTData1->OutlineWidth < 0 ? FOTData1->OutlineWidth = 0 : FOTData1->OutlineWidth;
	// Если нет Outline, то и корректору указываем что не нужно бодавлять пиксели к буквам
	FOTData1->Outline == false ? FOTData1->OutlineWidth = 0 : FOTData1->OutlineWidth;

	FOTData1->FontSize > 74.0f ? FOTData1->FontSize = 74.0f : FOTData1->FontSize;
	FOTData1->FontSize < 0.0f ? FOTData1->FontSize = 8.0f : FOTData1->FontSize;

	NewFont->FontSize = FOTData1->FontSize;

	CreateTextureFontFromFontFile ( FOTData1->FontName, FOTData1, NewFont, false, false );
	// Определяем размеры получившейся текстуры шрифта 
	CalcFTextureDimention ( PackHelper ( 0.0f, 0.0f, 0.0f, 0.0f ) );

	// Вносим коррекцию на Outline
	CorrectSymbolsDim ( FOTData1->OutlineWidth, FTextureWidth, SymbolsNumber, NewFont->Symbols );

	NewFont->FontHeightInPixel = FTextureHeight;	// Сохраняем максимальную высоту символа в шрифте

	TextureData NewTexture;

	NewTexture.Type = _2D;
	NewTexture.Usage = D3D11_USAGE_DEFAULT;
	NewTexture.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	NewTexture.Width = FTextureWidth;
	NewTexture.Height = FTextureHeight;
	NewTexture.Depth = 0;
	NewTexture.ShaderResource = true;	// Текстура будет создана как шейдер ресурс
	NewTexture.ArraySize = 1;
	NewTexture.MipMapLevels = 1;
	NewTexture.SampleDesc.Count = 1;
	NewTexture.SampleDesc.Quality = 0;
	NewTexture.GenerateMipMaps = false;
	NewTexture.RenderTarget = false;
	NewTexture.Shared_KeyMutex = false;
	NewTexture.TextureCube = false;
	NewTexture.DepthStensil = false;
	NewTexture.Unordered_Access = false;

	int TextureIndex = ResManager->CreateTexture ( NewTexture );

	if (TextureIndex < 0)
	{
		MessageBox ( 0, L"Could not initialize FontTexture.", Error, MB_OK );
		return nullptr;
	}

	NewFont->FontTextureIndex = TextureIndex; // Сохраняем индекс текстуры шрифта в данных шрифта

	// Копируем фонт в созданную текстуру FontTexture из которой будет производиться отрисовка символов по координатам
	D3D11_BOX* Box = new D3D11_BOX;
	ZeroMemory ( Box, sizeof ( D3D11_BOX ) );

	Box->top = Top;
	Box->left = 0;//Left;  Не используем Left потому, что мы не хотим обрезать текстуру фонта слева
	Box->bottom = Bottom;
	Box->right = Right;
	Box->front = 0;
	Box->back = 1;

	ID3D11Resource* tempResource;
	//  Получаем текстуру на которой мы рисовали текст, для того чтобы вырезать кусок содержащий ттекст
	d2dTexture->GetResource ( &tempResource );
	// Вырезаем кусок с текстом и рисекм им на тесктуре
	Local_D3DGC->DX_deviceContext->CopySubresourceRegion ( ResManager->TexturesArr[TextureIndex]->Texture2D, 0, 0, 0, 0, tempResource, 0, Box );
	// GetResource нужно делать Release
	tempResource->Release ();
	//	d3d11DevCon->CopyResource(FontTexture, D3DGC->sharedTex11);

	delete Box;

	if (SaveFont)
	{
		ResManager->SaveTextureToFile ( TextureIndex, L"Font.png" );
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Отремить если нужно сохранить файл с координатами символов в текстуре
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (SaveFontData)
	{
		std::ofstream fout;
		unsigned char *str1 = new unsigned char[2];
		//		std::string BigChar("A"); 

		std::wstring wst;

		str1[1] = '\0';

		// Пишем в файл	
		fout.open ( "Font.txt" );
		if (!fout.fail ())
		{
			fout.clear ();
			// Выводим символ - пробел
			fout << "32   0.0        0.0         0\n";

			for (UINT i = 0; i < NewFont->SymbolAmount; ++i)
			{
				fout << i + 33;
				fout << " ";
				if (i < 94)
					str1[0] = (unsigned char)(i + 0x21);
				else if (i == 158)
				{
					str1[0] = (unsigned char)(0xa8);
				}
				else if (i == 159)
				{
					str1[0] = (unsigned char)(0xb8);
				}
				else
				{
					str1[0] = (unsigned char)(i + 0x41 + 33);
				}
				fout << str1;
				fout << " ";
				fout << NewFont->Symbols[i].Start;
				fout << " ";
				fout << NewFont->Symbols[i].End;
				fout << " ";
				fout << (UINT)NewFont->Symbols[i].Symbol_Width;
				fout << "\n";
			}
		}
		fout.close ();
		delete[] str1;

	}

	return NewFont;
}


bool RenderClass::CreateTextureFontFromFontFile ( WCHAR* FontNameFullPath, RCube_Font_Style *FOTData, FontClass* NewFont, bool fontTextureOut, bool fontTextDescOut )
{

	bool result = true; // если всё классно, функция 

	IDWriteFontFile						   *g_pFontFile = nullptr;
	IDWriteFontFace							  *FontFace = nullptr;
	ID2D1SolidColorBrush			   *SolidColorBrush = nullptr;

	size_t size = wcslen ( NewFont->Str );

	WCHAR *Str_COPY = new WCHAR[size*2];

	wcscpy_s ( Str_COPY, size*2, NewFont->Str );

	UINT SymbolAmount = CalkNoSpaceAmount ( NewFont->Str );
	
	const float color_step = 0.1f;		// шаг в цветовой гамме ( 0.1 по умолчанию )

	hr = DWriteFactory->CreateFontFileReference ( FontNameFullPath, NULL, &g_pFontFile );
	if (FAILED ( hr ))
	{
		MessageBox ( NULL, L"Create font file reference failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	IDWriteFontFile* fontFileArray[] = { g_pFontFile };

	hr = DWriteFactory->CreateFontFace ( FOTData->FontFaceType, 1, fontFileArray, 0, DWRITE_FONT_SIMULATIONS_NONE, &FontFace );
	if (FAILED ( hr ))
	{
		MessageBox ( NULL, L"Create font file face failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	// генерируем цвета для каждого символа
	float  R = 1.0f;
	float  G = 1.0f;
	float  B = 1.0f;
	int edin = 10;
	int  ten = 10;
	int  sot = 10;

	//Release the D3D 11 Device
	keyedMutex11->ReleaseSync ( 0 );

	//Use D3D10.1 device
	keyedMutex10->AcquireSync ( 0, 0 );

	D2DRenderTarget->BeginDraw ();

	// First make sure the transformation is set to the identity transformation.
	// D2DRenderTarget->SetTransform ( D2D1::IdentityMatrix () );

	// Очищаем текстуру для рисования
	//	D2DRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	D2DRenderTarget->Clear ( D2D1::ColorF ( 0.0f, 0.0f, 0.0f, 0.0f ) );

	// Почему-то нужно сдвинуть текст вниз чтобы он разместился на текстуре
	D2DRenderTarget->SetTransform ( D2D1::Matrix3x2F::Translation ( 0.0f, FOTData->FontSize ) );  // 20 как раз для шрифта 36.0

																								  // Выключаем антиалиасинг, чтобы можно было распознать буквы по цвету
	D2DRenderTarget->SetAntialiasMode ( D2D1_ANTIALIAS_MODE_ALIASED );

	UINT L = 0; // Счётчик символов
	D2D1_COLOR_F FontColor; // Цвет которым красим символы

							// Создаём цвет для заливки им символов
	D2DRenderTarget->CreateSolidColorBrush ( D2D1::ColorF ( D2D1::ColorF::Plum ), // просто указали произвольный цвет при создании
		&SolidColorBrush
	);

	for (UINT i = 0; i < SymbolAmount; ++i)
	{

		if (edin == 0)
		{
			edin = 10;
			ten -= 1;
			R = 1.0f;
			G -= color_step;
		}
		if (ten == 0)
		{
			ten = 10;
			sot -= 1;
			G = 1.0f;
			B -= color_step;
		}
		if (sot == 0)
		{
			sot = 10;
			B = 1.0f;

		}

		//Set the Font Color
		FontColor = D2D1::ColorF ( R, G, B, 1.0f );

		// Сохраняем цвет в массив описывающий символ, чтобы потом можно было его найти по цвету в текстуре
		NewFont->Symbols[SymbolAmount - 1 - i].Color = PackHelper ( R, G, B, 1.0f );

		// Ставим цвет которым будем рисовать символ
		SolidColorBrush->SetColor ( FontColor );


		// +++++++++++++++++++++++    РИСУЕМ ТЕКСТ    ++++++++++++++++++++++++++++++++++++++++++++
		ID2D1PathGeometry					 *pPathGeometry = nullptr;
		ID2D1GeometrySink							 *pSink = nullptr;
		UINT								   *pCodePoints = nullptr;
		UINT16								 *pGlyphIndices = nullptr;

		L = (UINT)wcslen ( Str_COPY ); // Строка текста содержащая все нужные символы

		pCodePoints = new UINT[L];
		pGlyphIndices = new UINT16[L];

		ZeroMemory ( pCodePoints, sizeof ( UINT ) * L );
		ZeroMemory ( pGlyphIndices, sizeof ( UINT16 ) * L );

		UINT *Dest = &pCodePoints[0];
		wchar_t *Source = &NewFont->Str[0];
		for (UINT i = 0; i < L; ++i)
		{
			*Dest = *Source;
			++Dest;
			++Source;
		}

		hr = FontFace->GetGlyphIndicesW ( pCodePoints, L, pGlyphIndices );
		if (FAILED ( hr ))
		{
			MessageBox ( NULL, L"Create GetGlyphIndices failed!", Error, 0 );
			result = false;
			//			goto EXIT;
		}

		hr = D2DFactory->CreatePathGeometry ( &pPathGeometry );
		if (FAILED ( hr ))
		{
			MessageBox ( NULL, L"Create CreatePathGeometry failed!", Error, 0 );
			result = false;
			//			goto EXIT;
		}

		// Write to the path geometry using the geometry sink.
		hr = pPathGeometry->Open ( &pSink );
		if (FAILED ( hr ))
		{
			MessageBox ( NULL, L"Open pPathGeometry failed!", Error, 0 );
			result = false;
			//			goto EXIT;
		}

		hr = FontFace->GetGlyphRunOutline (
			FOTData->FontSize,//( 20.0f / 72.0f )*96.0f, // 36.0f - размер шрифта
			pGlyphIndices,
			NULL,
			NULL,
			L,
			FALSE,
			FALSE,
			pSink );
		if (FAILED ( hr ))
		{
			MessageBox ( NULL, L"GetGlyphRunOutline failed!", Error, 0 );
			result = false;
			//		goto EXIT;
		}

		hr = pSink->Close ();
		if (FAILED ( hr ))
		{
			MessageBox ( NULL, L"pSink close failed!", Error, 0 );
			result = false;
			//		goto EXIT;
		}

		// Draw text body
		D2DRenderTarget->FillGeometry ( pPathGeometry, SolidColorBrush );


		edin -= 1;
		R -= color_step;

		// Уменьшаем строку на один символ
		if (L > 2)
			Str_COPY[L - 2] = NULL;

		RCUBE_ARR_DELETE ( pCodePoints );
		RCUBE_ARR_DELETE ( pGlyphIndices );
		RCUBE_RELEASE ( pPathGeometry );
		RCUBE_RELEASE ( pSink );

	} // for

	RCUBE_ARR_DELETE( Str_COPY);

	// -------------------   КОНЕЦ рисования букв разным цветом   -------------------------------

	hr = D2DRenderTarget->EndDraw ();
	if (FAILED ( hr ))
	{
		MessageBox ( NULL, L"D2DRenderTarget->EndDraw() failed!", Error, 0 );
		result = false;
		//		goto EXIT;
	}

	//Release the D3D10.1 Device
	keyedMutex10->ReleaseSync ( 1 );

	//Use the D3D11 Device
	keyedMutex11->AcquireSync ( 1, 0 );

	// -----------------------------------------------------------
	// Если нужно сохранять цветную картинку
	Local_D3DGC->DX_deviceContext->CopyResource ( Created_textureBuf, sharedTex11 );

	D3D11_MAPPED_SUBRESOURCE  mapResource;

	hr = Local_D3DGC->DX_deviceContext->Map ( Created_textureBuf, 0, D3D11_MAP_READ, NULL, &mapResource );
	if (hr == S_OK)
	{
		int amount = mapResource.DepthPitch; // RowPitch * Height;
		int ui_amount = mapResource.DepthPitch / 4; // количество пикселей из 4 байт
		UINT* Pointer = (UINT*)mapResource.pData;

// +++++++++++++++++++++++++   Сохраняем цветной шрифт   +++++++++++++++++++++++++++++
/*
		Image img;
		D3D11_TEXTURE2D_DESC* TextureDesc = new D3D11_TEXTURE2D_DESC;
		sharedTex11->GetDesc( TextureDesc );
		img.height = TextureDesc->Height;
		img.width = TextureDesc->Width;
		img.format = TextureDesc->Format;
		img.rowPitch = mapResource.RowPitch;
		img.slicePitch = mapResource.DepthPitch;
		img.pixels = (UCHAR*)mapResource.pData;//block.data;
		hr = SaveToWICFile( img, WIC_FLAGS_NONE, GetWICCodec( WIC_CODEC_PNG ), L"ColourFont.png" );
		if ( FAILED( hr ) )
		{
			MessageBox( 0, L"Could not save font.PNG file.", Error, MB_OK );
		}
		delete TextureDesc;
*/
		// -------------------------   Сохраняем цветной шрифт   ----------------------------

// заполняем в массиве данных символов FirstPoint = true
		for (UINT i = 0; i < SymbolAmount; ++i)
		{
			NewFont->Symbols[i].FirstPoint = true;
		}

		// Размеры текстуры нужны чтобы правильно определять положение символа на текстуре
		int Columns = ui_amount / Height;
		int x = 0;
		int y = 0;
		int ActiveSymbol = 0;
		// сколько занимает один пиксель в текстуре

		double OnePixelLength = 1 / (double)(Width); // Берём ширину текстуры которая будет сохраняться в файл
		OnePixelLength *= 1000000000;
		OnePixelLength = floor ( OnePixelLength );
		OnePixelLength /= 1000000000;

		float OnePixL = (float)OnePixelLength;

		for (int t = 0; t < ui_amount; ++t)
		{

			if (Pointer[t] != 0)
			{
				unsigned int color = Pointer[t];
				for (UINT i = 0; i < SymbolAmount; ++i)
				{
					if (NewFont->Symbols[i].Color == color)
					{
						ActiveSymbol = i;
						break;
					}
				}

				_Symbol_Dim *Dest = &NewFont->Symbols[ActiveSymbol];

				// счётчик пикселей в символе
				++Dest->PointsAmount;
				// запоминаем самый левый пиксель в символе как самый левый и самый правый
				if (Dest->FirstPoint)
				{
					// самый левый пиксель
					Dest->Start_Pixel_XPos = x; // символ начинается с 0 по X ( в любом случае )
					Dest->Start_Pixel_YPos = y;
					// самый правый пиксель
					//				 Symbols[0].End_Pixel_XPos = x;
					//				 Symbols[0].End_Pixel_YPos = y;
					Dest->FirstPoint = false;
				}

				if (x < Dest->Start_Pixel_XPos)
					Dest->Start_Pixel_XPos = x;

				if (x > Dest->End_Pixel_XPos)
					Dest->End_Pixel_XPos = x;
				if (y > Dest->End_Pixel_YPos)
					Dest->End_Pixel_YPos = y;

			}
			++x;
			// счётчик строк и столбцов пикселей в текстуре
			// изменяем координаты следующего пикселя в текстуре
			if (x > Columns - 1)
			{
				++y;
				x = 0;
			}

			_Symbol_Dim *Dest = &NewFont->Symbols[ActiveSymbol];

			Dest->Symbol_Heigth = (float)Dest->End_Pixel_YPos - Dest->Start_Pixel_YPos; // +1
			Dest->Symbol_Width = (float)Dest->End_Pixel_XPos - Dest->Start_Pixel_XPos; // +1 

			Dest->Start = Dest->Start_Pixel_XPos * OnePixL;
			Dest->End = (Dest->End_Pixel_XPos + 1) * OnePixL;
		}

		Local_D3DGC->DX_deviceContext->Unmap ( Created_textureBuf, 0 );
	}


	// +++++++++++++++++++     Теперь уже рисуем полноценно с OUTLINE, если нужно     ++++++++++++++++++++++++++++++++++++++++

	ID2D1PathGeometry					 *pPathGeometry = nullptr;
	ID2D1GeometrySink							 *pSink = nullptr;
	UINT								   *pCodePoints = nullptr;
	UINT16								 *pGlyphIndices = nullptr;

	L = (UINT)wcslen ( NewFont->Str );

	pCodePoints = new UINT[L];
	pGlyphIndices = new UINT16[L];

	ZeroMemory ( pCodePoints, sizeof ( UINT ) * L );
	ZeroMemory ( pGlyphIndices, sizeof ( UINT16 ) * L );

	UINT *Dest = &pCodePoints[0];
	wchar_t *Source = &NewFont->Str[0];
	for (UINT i = 0; i < L; ++i)
	{
		*Dest = *Source;
		++Dest;
		++Source;
	}

	hr = FontFace->GetGlyphIndicesW ( pCodePoints, L, pGlyphIndices );
	if (FAILED ( hr ))
	{
		MessageBox ( NULL, L"Create GetGlyphIndices failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	hr = D2DFactory->CreatePathGeometry ( &pPathGeometry );
	if (FAILED ( hr ))
	{
		MessageBox ( NULL, L"Create CreatePathGeometry failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	// Write to the path geometry using the geometry sink.
	hr = pPathGeometry->Open ( &pSink );
	if (FAILED ( hr ))
	{
		MessageBox ( NULL, L"Open pPathGeometry failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	hr = FontFace->GetGlyphRunOutline (
		FOTData->FontSize,//( 20.0f / 72.0f )*96.0f, // 36.0f - размер шрифта
		pGlyphIndices,
		NULL,
		NULL,
		L,
		FALSE,
		FALSE,
		pSink );
	if (FAILED ( hr ))
	{
		MessageBox ( NULL, L"GetGlyphRunOutline failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	hr = pSink->Close ();
	if (FAILED ( hr ))
	{
		MessageBox ( NULL, L"pSink close failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	D2DRenderTarget->SetAntialiasMode ( FOTData->AliasingMode );

	//Release the D3D 11 Device
	keyedMutex11->ReleaseSync ( 0 );

	//Use D3D10.1 device
	keyedMutex10->AcquireSync ( 0, 0 );

	D2DRenderTarget->BeginDraw ();

	// First make sure the transformation is set to the identity transformation.
	//	D2DRenderTarget->SetTransform ( D2D1::IdentityMatrix () );

	// Clear the background of the window with a white color.
	//	D2DRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	D2DRenderTarget->Clear ( D2D1::ColorF ( 0.0f, 0.0f, 0.0f, 0.0f ) );

	// shift the text down
	D2DRenderTarget->SetTransform ( D2D1::Matrix3x2F::Translation ( 0.0f, FOTData->FontSize ) );

	if (FOTData->Outline)
	{
		// Draw text outline
		D2DRenderTarget->DrawGeometry ( pPathGeometry, FOTData->OutlineBrush, (float)FOTData->OutlineWidth );
	}

	// Draw text body
	D2DRenderTarget->FillGeometry ( pPathGeometry, FOTData->FillBrush );

	hr = D2DRenderTarget->EndDraw ();
	if (FAILED ( hr ))
	{
		MessageBox ( NULL, L"D2DRenderTarget->EndDraw() failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	//Release the D3D10.1 Device
	keyedMutex10->ReleaseSync ( 1 );

	//Use the D3D11 Device
	keyedMutex11->AcquireSync ( 1, 0 );

	RCUBE_ARR_DELETE ( pCodePoints );
	RCUBE_ARR_DELETE ( pGlyphIndices );
	RCUBE_RELEASE ( pPathGeometry );
	RCUBE_RELEASE ( pSink );
/*
	if ( fontTextureOut )
	{
	// Если нужно сохранять ШРИФТ НА ТЕКСТУРЕ в ФАЙЛ
	Local_D3DGC->DX_deviceContext->CopyResource( Created_textureBuf, sharedTex11 );

	hr = Local_D3DGC->DX_deviceContext->Map( Created_textureBuf, 0, D3D11_MAP_READ, NULL, &mapResource );
	if (hr == S_OK)
	{

	int amount = mapResource.DepthPitch; // RowPitch * Height;
	int ui_amount = mapResource.DepthPitch / 4; // количество пикселей из 4 байт

	Image img;
	D3D11_TEXTURE2D_DESC* TextureDesc = new D3D11_TEXTURE2D_DESC;

	sharedTex11->GetDesc( TextureDesc );
	img.height = TextureDesc->Height;
	img.width = TextureDesc->Width;
	img.format = TextureDesc->Format;
	img.rowPitch = mapResource.RowPitch;
	img.slicePitch = mapResource.DepthPitch;
	img.pixels = (UCHAR*)mapResource.pData;

	hr = SaveToWICFile( img, WIC_FLAGS_NONE, GetWICCodec( WIC_CODEC_PNG ), L"Font1.png" );
	if ( FAILED( hr ) )
	{
		MessageBox( 0, L"Could not save font.PNG file.", Error, MB_OK );
	}

	delete TextureDesc;

	Local_D3DGC->DX_deviceContext->Unmap ( Created_textureBuf, 0 );
	}

	}
*/
/*
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Отремить если нужно сохранить файл с координатами символов в текстуре
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if ( fontTextDescOut )
	{
	std::ofstream fout;
	unsigned char *str1 = new unsigned char[2];
	//		std::string BigChar("A");

	std::wstring wst;

	str1[1] = '\0';

	// Пишем в файл
	fout.open( "Font.txt" );
	if ( !fout.fail() )
	{
	fout.clear();
	// Выводим символ - пробел
	fout << "32   0.0        0.0         0\n";

	for ( int i = 0; i < SymbolAmount; ++i )
	{
	fout << i + 33;
	fout << " ";
	if ( i < 94 )
	str1[0] = ( unsigned char ) ( i + 0x21 );
	else if ( i == 158 )
	{
	str1[0] = ( unsigned char ) ( 0xa8 );
	}
	else if ( i == 159 )
	{
	str1[0] = ( unsigned char ) ( 0xb8 );
	}
	else
	{
	str1[0] = ( unsigned char ) ( i + 0x41 + 33 );
	}
	fout << str1;
	fout << " ";
	fout << Symbols[i].Start;
	fout << " ";
	fout << Symbols[i].End;
	fout << " ";
	fout << ( UINT ) Symbols[i].Symbol_Width;
	fout << "\n";
	}
	}
	fout.close();
	delete[] str1;
	}
	// -------------------------------------------------------------------------
*/

EXIT:

	RCUBE_RELEASE ( SolidColorBrush );
	RCUBE_RELEASE ( FontFace );
	RCUBE_RELEASE ( g_pFontFile );

	return result;
}


RCube_Font_Style* RenderClass::Create_RCube_FontStyle ( Font_Param* Fparam )
{
	ID2D1LinearGradientBrush	*LinearGradientBrush = nullptr;
	ID2D1SolidColorBrush		  *SolidBrushOutline = nullptr;
	ID2D1GradientStopCollection		  *GradientStops = nullptr;

	RCube_Font_Style* Temp_Font_Style = new RCube_Font_Style;

	Temp_Font_Style->AliasingMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;// Включаем алиасинг
	Temp_Font_Style->FontFaceType = DWRITE_FONT_FACE_TYPE_TRUETYPE;

	Temp_Font_Style->FontSize = Fparam->FontSize * Local_D3DGC->ScreenScale.w; // Apply Scale according to screen resolution
	Temp_Font_Style->FontSize < 6 ? Temp_Font_Style->FontSize = 6 : Temp_Font_Style->FontSize;

	int Namesize = lstrlenW ( Fparam->Font_Path_Name );
	wchar_t *NamePointer = new wchar_t[Namesize + 1];
	Temp_Font_Style->FontName = NamePointer;
	lstrcpyW ( NamePointer, Fparam->Font_Path_Name );

	Temp_Font_Style->Outline = Fparam->Outlined;
	Temp_Font_Style->OutlineWidth = Fparam->OutlineWidth;

	hr = Local_D3DGC->D2DRenderTarget->CreateGradientStopCollection (
		&Fparam->gradientStops[0],
		3,
		D2D1_GAMMA_2_2,
		D2D1_EXTEND_MODE_CLAMP,
		&GradientStops
	);
	if ( FAILED ( hr ) )
	{
		MessageBox ( NULL, L"Create GradientStopCollection failed!", Error, 0 );
	}

	hr = Local_D3DGC->D2DRenderTarget->CreateLinearGradientBrush (
		D2D1::LinearGradientBrushProperties (
		D2D1::Point2F ( Fparam->GradientPos.x, Fparam->GradientPos.y ),
		D2D1::Point2F ( Fparam->GradientPos.z, Fparam->GradientPos.w ) ),
		GradientStops,
		&LinearGradientBrush
	);
	if ( FAILED ( hr ) )
	{
		MessageBox ( NULL, L"Create LinearGradientBrush failed!", Error, 0 );
	}

	hr = Local_D3DGC->D2DRenderTarget->CreateSolidColorBrush ( D2D1::ColorF (
		Fparam->OutlineColor.x,
		Fparam->OutlineColor.y,
		Fparam->OutlineColor.z,
		Fparam->OutlineColor.w ),
		&SolidBrushOutline
	);
	if ( FAILED ( hr ) )
	{
		MessageBox ( NULL, L"Create SolidColorBrush failed!", Error, 0 );
	}

	Temp_Font_Style->OutlineBrush = SolidBrushOutline;
	Temp_Font_Style->FillBrush = LinearGradientBrush;
	RCUBE_RELEASE ( GradientStops );


	return Temp_Font_Style;

}


void RenderClass::RenderSentence ( SentenceType* sentence )
{
	unsigned int stride, offset;

	stride = sizeof ( Vertex_FlatObject );
	offset = 0;

	Local_D3DGC->DX_deviceContext->IASetVertexBuffers ( 0, 1, &ResManager->FlatObjectBuffers[sentence->VertexBufferIndex]->FlatObjectVB->Buffer, &stride, &offset );

	Local_D3DGC->DX_deviceContext->PSSetShaderResources ( 0, 1, &ResManager->TexturesArr[ResManager->RCube_Font[sentence->FontType]->FontTextureIndex]->SRV );

	Local_D3DGC->DX_deviceContext->Draw ( sentence->vertexCount, 0 );

}


void RenderClass::ShowGlowing ( SentenceType* Source )
{
	RCube_VecFloat234 TextV34;

	if ( Source->ShowGoOn == false )
	{
		if ( Source->Colour.w > 0.0f )
			Source->Colour.w -= fpstimers.FrameTime * 2.0f; // How fast to glow
		else
			Source->ShowGoOn = true;
	}
	else
	{
		if ( Source->Colour.w < 1.0f )
			Source->Colour.w += fpstimers.FrameTime * 2.0f; // How fast to glow
		else
			Source->ShowGoOn = false;
	}
}


void RenderClass::ShowFromDim ( SentenceType* Source )
{
	RCube_VecFloat234 TextV34;

	if ( Source->FromDim == false )
	{
		if ( Source->Colour.w < 1.0f )
			Source->Colour.w += fpstimers.FrameTime * 1.0f; // How fast to light string

		Source->Colour.w > 1.0f ? Source->Colour.w = 1.0f : Source->Colour.w;
	}
	else
	{
		if ( Source->Colour.w > 0.0f )
			Source->Colour.w -= fpstimers.FrameTime * 1.0f; // How fast to light string

		Source->Colour.w < 0.0f ? Source->Colour.w = 0.0f : Source->Colour.w;
	}
}


void RenderClass::HideToDim ( SentenceType* Source )
{
	RCube_VecFloat234 TextV34;

	if ( Source->ShowGoOn == true )
	{
		if ( Source->Colour.w > 0.0f )
		{
			Source->Colour.w -= fpstimers.FrameTime * 1.0f; // How fast to light string
		}
		else
		{
			Source->Colour.w = 0.0f;
//			Source->ShowGoOn = false;	// Stop apply effect
			Source->Render = false;		// Stop render sentence
		}
	}
}


void RenderClass::ShowScrolling ( SentenceType* Source )
{

	if (Source->ScrollGoOn == false)
	{
		Source->PosY = Local_D3DGC->ScreenHeight - (int)ResManager->RCube_Font[Source->FontType]->FontSize;

		Source->TempValScroll = 0.0f;

		Source->ScrollGoOn = true;
	}
	else
	{
		Source->TempValScroll += fpstimers.FrameTime;

		if (Source->TempValScroll > (1.0f / Source->ScrollSpeed))
		{
			--Source->PosY;
			Source->TempValScroll = 0.0f;
		}
		if (Source->PosY == 0)
			Source->ScrollGoOn = false;

//		ResManager->UpdateSentence(i, ResManager->RCube_Sentences[i]->Text, ResManager->RCube_Sentences[i]->PosX, ResManager->RCube_Sentences[i]->PosY);
	}
}


void RenderClass::RenderText ( int Level )
{
	bool GLOW = false;
	bool SCROLL = false;
	bool FROMDIM = false;
	bool HIDETODIM = false;

//	Local_D3DGC->DX_deviceContext->IASetPrimitiveTopology ( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	UINT size = ( UINT ) ResManager->RCube_Sentences.size ();
	for (UINT i = 0, j = size; i < j; ++i)
	{
		SentenceType* Source = ResManager->RCube_Sentences[i];
		if (Source->Render == true && Source->Level == Level)
		{
			Source->ShowType & SHOW_GLOWING ? GLOW = true : GLOW = false;
			Source->ShowType & SHOW_SCROLLING ? SCROLL = true : SCROLL = false;
			Source->ShowType & SHOW_FROM_DIM ? FROMDIM = true : FROMDIM = false;
			Source->ShowType & HIDE_TO_DIM ? HIDETODIM = true : HIDETODIM = false;


			if ( GLOW )
				ShowGlowing ( Source );

			if ( SCROLL )
				ShowScrolling ( Source );

			if ( FROMDIM )
				ShowFromDim ( Source );

			if ( HIDETODIM )
				ShowFromDim ( Source );
	
			RenderSentence ( Source );
		}
	}

}


// Если Shadows = 1 , то ObjectIndex рендерится в ShadowMap
void RenderClass::Render3D_Object ( int ObjectIndex, bool Shadows )
{

	UINT c = 0;

	UINT strides[2];
	UINT offsets[2];

	ID3D11Buffer* bufferPointers[2];

	_3DModel* Model = ResManager->Get3D_Object_Pointer ( ObjectIndex );

	if ( Model->InstanceFrustumPassedAmount > 0 )
	{

		// Set vertex buffer stride and offset.
		strides[0] = sizeof ( Vertex_Model3D );
		strides[1] = sizeof ( PositionType );

		// Set the buffer offsets.
		offsets[0] = 0;
		offsets[1] = 0;

		bufferPointers[1] = Model->InstancePositionsBuffer->GetBuffer ();// m_posesBuffer;

		size_t MeshCounter = Model->Meshes.size ();

		while ( c < MeshCounter )
		{

			// Set the array of pointers to the vertex and instance buffers.
			_3D_Obj_Buffers* Buffers = ResManager->Get_Mesh_Buffers_By_Index ( Model->Meshes[c]->BufferIndex );

			bufferPointers[0] = Buffers->Vertexes->Buffer;

			// Set the vertex buffer to active in the input assembler so it can be rendered.
			Local_D3DGC->DX_deviceContext->IASetVertexBuffers ( 0, 2, bufferPointers, strides, offsets );

			if ( Shadows )
			{
				if ( Model->InstanceFrustumPassedAmount > 1 )
					Local_D3DGC->DX_deviceContext->DrawInstanced ( Model->Meshes[c]->VertexBufferSize, Model->InstanceFrustumPassedAmount, 0, 0 );
				else
					Local_D3DGC->DX_deviceContext->Draw ( Model->Meshes[c]->VertexBufferSize, 0 );
			}
			else
			{
				Local_D3D->RenderClustered ( ResManager->TexturesArr[Model->Meshes[c]->TextureIndex]->SRV, Model->Meshes[c]->VertexBufferSize, Model->InstanceFrustumPassedAmount );
			}
			++c;
		}
	}
}


// Если Shadows = 1 , то ObjectIndex рендерится в ShadowMap
void RenderClass::RenderTerrain ( int ObjectIndex, bool Shadows )
{
	UINT strides[2];
	UINT offsets[2];
	ID3D11Buffer* bufferPointers[2];

	// Set vertex buffer stride and offset.
	strides[0] = sizeof ( Vertex_Model3D );
	strides[1] = sizeof ( PositionType );

	// Set the buffer offsets.
	offsets[0] = 0;
	offsets[1] = 0;

	Terrain* _Terrain = ResManager->Get_Terrain_Object_Pointer ( ObjectIndex );

	_3D_Obj_Buffers* Buffers = ResManager->Get_Terrain_Buffers_By_Index ( _Terrain->TerrainBuffersIndex );

	bufferPointers[0] = Buffers->Vertexes->Buffer;
	bufferPointers[1] = _Terrain->InstancePositionsBuffer->Buffer;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	Local_D3DGC->DX_deviceContext->IASetVertexBuffers ( 0, 2, bufferPointers, strides, offsets );

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
//	Local_D3DGC->DX_deviceContext->IASetPrimitiveTopology ( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Set shader texture resource in the pixel shader.
	Local_D3DGC->DX_deviceContext->PSSetShaderResources ( 0, 1, &ResManager->TexturesArr[_Terrain->TerrainTextureIndex]->SRV );

	if ( _Terrain->ClusterRender )
	{
		_3D_Obj_Buffers* ClusterBuffers;
		size_t Amount = _Terrain->PassedFrustumClasters.size ();
		for ( size_t i = 0; i < Amount; ++i )
		{
			ClusterBuffers = ResManager->Get_Terrain_Buffers_By_Index ( _Terrain->PassedFrustumClasters[i]->ClusterBufferIndex );
			Local_D3DGC->DX_deviceContext->IASetIndexBuffer ( ClusterBuffers->Indexes->Buffer, DXGI_FORMAT_R32_UINT, 0 );

			if ( Shadows )
			{
				Local_D3DGC->DX_deviceContext->DrawIndexed ( _Terrain->Total_Indexs_In_Cluster, 0, 0 );
			}
			else
			{
				Local_D3D->RenderIndextedClustered ( ResManager->TexturesArr[_Terrain->TerrainTextureIndex]->SRV, _Terrain->Total_Indexs_In_Cluster, 1 );
			}
		}
	}
	else
	{
		// Set the index buffer to active in the input assembler so it can be rendered.
		Local_D3DGC->DX_deviceContext->IASetIndexBuffer ( Buffers->Indexes->Buffer, DXGI_FORMAT_R32_UINT, 0 );
		if ( Shadows )
		{
			Local_D3DGC->DX_deviceContext->DrawIndexed ( _Terrain->Total_Indexs_In_Terrain, 0, 0 );
		}
		else
		{
			Local_D3D->RenderIndextedClustered ( ResManager->TexturesArr[_Terrain->TerrainTextureIndex]->SRV, _Terrain->Total_Indexs_In_Terrain, 1 );
		}
	}

}


void RenderClass::RenderCubeMap ( int ObjectIndex )
{

	UINT stride = sizeof ( Vertex_FlatObject );
	UINT offset = 0;
//	Local_D3DGC->DX_deviceContext->IASetPrimitiveTopology ( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	Local_D3DGC->DX_deviceContext->IASetIndexBuffer ( ResManager->FlatObjectBuffers[ResManager->CubeMaps[ObjectIndex]->CubeMapBuffersIndex]->IndexBs->Buffer, DXGI_FORMAT_R32_UINT, 0 );

	Local_D3DGC->DX_deviceContext->IASetVertexBuffers ( 0, 1, &ResManager->FlatObjectBuffers[ResManager->CubeMaps[ObjectIndex]->CubeMapBuffersIndex]->FlatObjectVB->Buffer, &stride, &offset );

	Local_D3DGC->DX_deviceContext->PSSetShaderResources ( 0, 1, &ResManager->TexturesArr[ResManager->CubeMaps[ObjectIndex]->CubeMapTextureIndex]->SRV );

	ResManager->SetActiveShadersInProgramm ( ResManager->CubeMaps[ObjectIndex]->ShaderIndex );

	Local_D3DGC->DX_deviceContext->OMSetDepthStencilState ( Local_D3DGC->CubeMap_DepthStencil_State, 0 );

	Local_D3DGC->DX_deviceContext->DrawIndexed ( ResManager->CubeMaps[ObjectIndex]->NumSphereFaces, 0, 0 );

}


void RenderClass::Init_ShadowMap_RasterizerState ( int DepthBias, float SlopeScaledDepthBias )
{
	RCUBE_RELEASE ( Local_D3DGC->LightRender_RS );
	CD3D11_RASTERIZER_DESC desc ( D3D11_DEFAULT );
	desc.CullMode = D3D11_CULL_FRONT; //D3D11_CULL_BACK; //D3D11_CULL_FRONT; //D3D11_CULL_NONE;//
	desc.FillMode = D3D11_FILL_SOLID;//D3D11_FILL_WIREFRAME;//D3D11_FILL_SOLID;//
	desc.FrontCounterClockwise = false;
	desc.ScissorEnable = false;
	desc.DepthClipEnable = true;//true;
	desc.MultisampleEnable = true;//false;
	desc.AntialiasedLineEnable = true;
	desc.DepthBias = DepthBias;//1.e5;//0.0f;//1.e5;
	desc.SlopeScaledDepthBias = SlopeScaledDepthBias;// 1.0f;//8.0;//0.0f;//8.0;
	desc.DepthBiasClamp = 1.0f;
	Local_D3DGC->DX_device->CreateRasterizerState ( &desc, &Local_D3DGC->LightRender_RS );
}


void RenderClass::DrawObjectUsingShadows ( XMVECTOR DrawPosition )
{
	// 3D Modek render if want to cast shadows
	ResManager->Frustum_3D_Objects ( Local_Frustum );
	int Objects_Amount = ( int ) ResManager->_3DModels.size ();
	for ( int i = 0; i < Objects_Amount; ++i )
	{
		Render3D_Object ( i, true );
	}

	// Terrain render if want to cast shadows
	Objects_Amount = ( int ) ResManager->Terrains.size ();
	for ( int i = 0; i < Objects_Amount; ++i )
	{
		Terrain* Terrain = ResManager->Terrains[i];

		if ( Terrain->CastShadow )
		{
			if ( Terrain->ClusterRender )
				ResManager->FrustumTerrain ( Local_Frustum, Terrain);

			RenderTerrain ( i, true );
		}
	}
}


void RenderClass::RenderSpotLightsSadowMaps ( std::vector <int>* SpotLightsWithShadowsIndexes )
{

	HRESULT res;

	CB_ShadowMap cbPerObj;


	ID3D11ShaderResourceView * tab[1];
	tab[0] = NULL;
	Local_D3DGC->DX_deviceContext->PSSetShaderResources ( 1, 1, tab );
	Local_D3DGC->DX_deviceContext->PSSetShader ( 0, 0, 0 );

	//	Local_D3DGC->DX_deviceContext->VSSetShader(myManeger->VertShaderArr[ShaderFordraw], 0, 0);
	Local_D3DGC->DX_deviceContext->RSSetState ( Local_D3DGC->LightRender_RS );
	Local_D3DGC->DX_deviceContext->GSSetShader ( 0, 0, 0 );
	Local_D3DGC->DX_deviceContext->OMSetDepthStencilState ( Local_D3DGC->LightRender_DS, 0 );
	// Устанавливаем ViewPort для ShadowMap
	Local_D3DGC->DX_deviceContext->RSSetViewports ( 1, &Local_D3D->LightViewPort );
	//	g_D3DGC->DX_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	int Amount = ( int ) SpotLightsWithShadowsIndexes->size ();
	for ( int i = 0; i < Amount; i++ )
	{

		// Для скорости убрал всё что не меняется в глобальную инициализацию
		Local_D3DGC->MSAAQualityCount > 1 ?	DSD2.Texture2DMSArray.FirstArraySlice = i
										  : DSD2.Texture2DArray.FirstArraySlice   = i;
		

		res = Local_D3DGC->DX_device->CreateDepthStencilView ( Local_D3D->ShadowMap3D, &DSD2, &Local_D3DGC->DSV_ShadowMap3D );
		//if (FAILED(res))
		//{
		//	return false;
		//}
#if defined( DEBUG ) || defined( _DEBUG )
		const char c_szName [] = "DSV_ShadowMap3D";
		Local_D3DGC->DSV_ShadowMap3D->SetPrivateData ( WKPDID_D3DDebugObjectName, sizeof ( c_szName ) - 1, c_szName );
#endif
		// Для скорости создаём указатель
		int* Point = &SpotLightsWithShadowsIndexes[0][i];
		PointLight* Point1 = Local_D3D->mPointLightParameters[*Point];

		LightPosition.Fl3 = Point1->position;
		//			LightPosition.Fl4.w = 0.0f;
		LightTarget.Fl3 = Point1->direction;
		//			LightTarget.Fl4.w = 0.0f;
		//			LightTarget.Vec = LightTarget.Vec / 100.0f;
		//			XMVECTOR LightTarget = XMLoadFloat3(&g_Light->mPointLightParameters[*Point]->direction);
		// http://www.3dgep.com/understanding-the-view-matrix/
		// http://www.codinglabs.net/article_world_view_projection_matrix.aspx
		XMMATRIX LightView = XMMatrixLookToLH ( LightPosition.Vec, LightTarget.Vec, Up ); //XMVector3Normalize  / XMVector3NormalizeEst

		XMMATRIX LightProjection = XMMatrixPerspectiveFovLH ( XM_PIDIV4, Local_D3DGC->ScreenRatio,
															  Local_D3DGC->NearPlane, Point1->attenuationEnd ); //m_Light->D3DGC_Light->FarPlane * 10
																												//		LightProjection = XMMatrixOrthographicLH( g_D3DGC->ScreenWidth, g_D3DGC->ScreenHeight, g_D3DGC->NearPlane, Point1->attenuationEnd );

		XMMATRIX LightViewProj = LightView * LightProjection;
		// Это пока не нужно.  У нас пока нет отсеивание объектов по светам. Это нужно не здесь.
		Local_Frustum->ConstructFrustumNew ( LightViewProj );

		cbPerObj.LightViewProjection = ( LightViewProj ); //XMMatrixTranspose 

//		Local_D3DGC->DX_deviceContext->UpdateSubresource ( cbShadowBuffer, 0, NULL, &cbPerObj, 0, 0 );
		Local_D3DGC->ShadowMapLightView->Update ( &cbPerObj );
		Local_D3DGC->DX_deviceContext->VSSetConstantBuffers ( 1, 1, &Local_D3DGC->ShadowMapLightView->Buffer );
		// Создаём матрицу поворота
		RCubeMatrix Mat;
		Mat.XMM = cbPerObj.LightViewProjection;
		Point1->qtwvp = Mat.XMF;
		//		XMStoreFloat4x4(&Point1->qtwvp, cbPerObj.ViewProjection );
		// ----------------------------------------------------------------------------------
		// Создаём кватернион поворота
		//		LightTarget.Vec = XMQuaternionRotationMatrix( cbPerObj.ViewProjection );
		//		Point1->RotQuat = LightTarget.Fl4;

		//		cbPerObj.ViewProjection = XMMatrixRotationQuaternion(LightTarget.Vec);
		// ----------------------------------------------------------------------------------
		Local_D3DGC->DX_deviceContext->OMSetRenderTargets ( 0, 0, Local_D3DGC->DSV_ShadowMap3D );
		Local_D3DGC->DX_deviceContext->ClearDepthStencilView ( Local_D3DGC->DSV_ShadowMap3D, D3D11_CLEAR_DEPTH, 1.0f, 0 );
		DrawObjectUsingShadows ( LightPosition.Vec );

		// Сохраняем в световом массиве индекс куска Shadow Map
		Point1->ShadowMapSliceNumber = float(i);
		Point1->LightID = i;
//		Local_D3DGC->DSV_ShadowMap3D->Release ();
		RCUBE_RELEASE ( Local_D3DGC->DSV_ShadowMap3D );
	}
	// Устанавливаем ViewPort для RCube
	Local_D3DGC->DX_deviceContext->RSSetViewports ( 1, &Local_D3D->viewport );
}


void RenderClass::RenderParticleSystem ( int Index )
{
	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];

	// Set the buffer offsets.
	offsets[0] = 0;
	offsets[1] = 0;

	int Size = ( int ) ResManager->ParticleSystems.size ();

	if ( Index < Size )
	{
		ParticleSystem* TempSystem = ResManager->ParticleSystems[Index];

		if ( TempSystem->Render != false )
		{
			size_t EmittersAmount = ResManager->ParticleSystems[Index]->Emitters.size ();

			for ( size_t c = 0; c < EmittersAmount; ++c )
			{
				
				Emitter* TempEmitter = TempSystem->Emitters[c];

				if ( TempEmitter->Active )
				{
					ResManager->SetActiveShadersInProgramm ( TempEmitter->Init_Data.ShaderForDraw );
					
					switch ( TempEmitter->Type )
					{
						case EM_BILLBOARD:
						{
							// Set vertex buffer stride and offset.
							strides[0] = sizeof ( Vertex_FlatObject );
							strides[1] = sizeof ( BB_Particle_Instance );

							BB_Particles_Buffers* TempBuffers = ResManager->GetEmitters_BB_Buffers_By_Index ( TempEmitter->Emitter_Buffers_Index );
							// Set the array of pointers to the vertex and instance buffers.
							bufferPointers[0] = TempBuffers->FlatObjectVB->Buffer;// m_vertexBuffer;
							bufferPointers[1] = TempBuffers->InstanceVBs->Buffer;// m_instanceBuffer;

																				 // Set the vertex buffer to active in the input assembler so it can be rendered.
							Local_D3DGC->DX_deviceContext->IASetVertexBuffers ( 0, 2, bufferPointers, strides, offsets );

							// Set the index buffer to active in the input assembler so it can be rendered.
							Local_D3DGC->DX_deviceContext->IASetIndexBuffer ( TempBuffers->IndexBs->Buffer, DXGI_FORMAT_R32_UINT, 0 );

							// Set the type of primitive that should be rendered from this vertex buffer.
							//	D3DGC_Local->DX_deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

							// Set shader texture resource in the pixel shader.
							Local_D3DGC->DX_deviceContext->PSSetShaderResources ( 0, 1, &TempBuffers->RenderTexture );

							// Render the triangle.
							Local_D3DGC->DX_deviceContext->DrawIndexedInstanced ( 6, TempEmitter->CreatedParticlesCount, 0, 0, 0 );

							break;
						}
					}
				}
			}
		}
	}
}
#pragma once
#include "stdafx.h"

#include "RenderClass.h"

RenderClass::RenderClass ( D3DGlobalContext* GlobalContext, ResourceManager * _ResManager, D3DClass* m_D3D, TextClass* _text )
{
	ResManager  = _ResManager;
	Local_D3DGC =  GlobalContext;
	Local_D3D   =  m_D3D;
	Global_text = _text;

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
}


RenderClass::~RenderClass ()
{

}


void RenderClass::RenderMenu (int Index)
{
	size_t c = ResManager->Menus.size ();
	
	Menu *TempMenu = ResManager->Menus[Index];

	if (Index < c)
	{
		// Если есть фон, рисуем его
		if (TempMenu->Background)
		{
			Flat_Obj_Buffers* FlatObject = ResManager->Get_Flat_ObjectBuffers_ByIndex ( TempMenu->Background->Buffers->ThisBufferIndex );
			RenderFlatObject ( FlatObject );
		}

		// Если есть анимация в меню, то рисуем её
		if (TempMenu->Animation != nullptr && TempMenu->Animation->Enabled )
		{
			Flat_Obj_Buffers* FlatObject = ResManager->Get_Flat_ObjectBuffers_ByIndex ( TempMenu->Animation->AnimeTextures->Buffers->ThisBufferIndex );
			// Устанавливаем специфический шейдер для анимации
			ResManager->SetActiveShadersInProgramm ( TempMenu->Animation->ShaderForDraw );
			RenderFlatObject ( FlatObject );
			// Возвращаем шейдер для отрисовки остальных элементов меню
			ResManager->SetActiveShadersInProgramm ( TempMenu->Animation->ShaderReturnTo );
		}
		// Buttons
		int c = 0;
		while (c < TempMenu->g_NumOfButtons)
		{
			Flat_Obj_Buffers* FlatObject = ResManager->Get_Flat_ObjectBuffers_ByIndex ( TempMenu->Buttons[c]->Buffers->ThisBufferIndex );
			RenderFlatObject ( FlatObject );
			++c;
		}
		// Scrollbars
		c = 0;
		while (c < TempMenu->g_NumOfScrollBars)
		{
			Flat_Obj_Buffers* FlatObject;

			FlatObject = ResManager->Get_Flat_ObjectBuffers_ByIndex ( TempMenu->ScrollBars[c]->Body->Buffers->ThisBufferIndex );
			RenderFlatObject ( FlatObject );
			// исуем три кнопки от скролбара
			if (TempMenu->ScrollBars[c]->MinMixButtonsShow)
			{
				FlatObject = ResManager->Get_Flat_ObjectBuffers_ByIndex ( TempMenu->ScrollBars[c]->MinButton->Buffers->ThisBufferIndex );
				RenderFlatObject ( FlatObject );
				FlatObject = ResManager->Get_Flat_ObjectBuffers_ByIndex ( TempMenu->ScrollBars[c]->MaxButton->Buffers->ThisBufferIndex );
				RenderFlatObject ( FlatObject );
			}

			FlatObject = ResManager->Get_Flat_ObjectBuffers_ByIndex ( TempMenu->ScrollBars[c]->Traveller->Buffers->ThisBufferIndex );
			RenderFlatObject ( FlatObject );
			++c;
		}
		// StringLists
		c = 0;
		while (c < TempMenu->g_NumOfStringsLists)
		{
			Local_D3DGC->DX_deviceContext->ClearRenderTargetView ( Local_D3DGC->BackBuffer_ProxyTextureRTV, Local_D3DGC->ZeroColor );
			Local_D3DGC->DX_deviceContext->OMSetRenderTargets ( 1, &Local_D3DGC->BackBuffer_ProxyTextureRTV, NULL );

			// Рисуем строки в текстуру для скролинга
			Global_text->Render ( TempMenu->StringsList[c]->SentencesIndex );

			// Сохраняем в файл для визуальной отладки
			//	SaveTextureToPNG( StrList_SRV );

			Local_D3DGC->DX_deviceContext->OMSetRenderTargets ( 1, &Local_D3DGC->BackBuffer_RTV, Local_D3DGC->m_depthStencilView );

			Flat_Obj_Buffers* FlatObject = ResManager->Get_Flat_ObjectBuffers_ByIndex ( TempMenu->StringsList[c]->Buffers->ThisBufferIndex );
			RenderFlatObject ( FlatObject );
			++c;
		}
	}

}


void RenderClass::RenderScene ()
{

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

	Local_D3DGC->DX_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Local_D3DGC->DX_deviceContext->PSSetShaderResources ( 0, 1, &FlatObject->RenderTexture );

	Local_D3DGC->DX_deviceContext->IASetInputLayout( ResManager->Layouts[FLATOBJECT] );

	//	D3DGC->DX_deviceContext->PSSetSamplers(0, 1, &m_sampleState);

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


void RenderClass::CalcFTextureDimention ( UINT FonColour )
{
	Local_D3DGC->DX_deviceContext->CopyResource ( Created_textureBuf, sharedTex11 );

	D3D11_MAPPED_SUBRESOURCE  mapResource;

	hr = Local_D3DGC->DX_deviceContext->Map ( Created_textureBuf, 0, D3D11_MAP_READ, NULL, &mapResource );
	if (!FAILED ( hr ))
	{

		union _block
		{
			byte  *data;
			UINT *udata;
		} block;


		int amount = mapResource.DepthPitch; // RowPitch * Height;
		int ui_amount = mapResource.DepthPitch / 4; // количество пикселей из 4 байт
		block.data = new byte[amount];
		// инициализируем массив
		memcpy ( block.data, mapResource.pData, amount );

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
			if (block.udata[i] != 0 && block.udata[i] != FonColour)
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
		/*	   Image img;
		img.width = FTextureWidth;
		img.height = FTextureHeight;
		img.format = DXGI_FORMAT_R8G8B8A8_UNORM;
		img.rowPitch = mapResource.RowPitch;
		img.slicePitch = mapResource.DepthPitch;
		img.pixels = block.data;
		hr = SaveToWICFile(img, WIC_FLAGS_NONE, GetWICCodec(WIC_CODEC_PNG), L"222.png");
		if (FAILED(hr))
		{
		MessageBox(0, L"Could not save font.PNG file.", Error, MB_OK);
		}
		*/

		delete[] block.data;
	}
	Local_D3DGC->DX_deviceContext->Unmap ( Created_textureBuf, 0 );

}


bool RenderClass::RenderFontOnTexture ( ID3D11ShaderResourceView* textureForDraw,
	WCHAR* FontNameFullPath,
	WCHAR* text,
	FontOnTextureData*FOTData
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
		D2DRenderTarget->DrawGeometry ( pPathGeometry, FOTData->OutlineBrush, FOTData->OutlineWidth );
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

	int Res1 = ResManager->Create_Flat_Obj_Buffers ( D3D11_USAGE_DEFAULT, 1, 6, nullptr );
	int Res2 = ResManager->Create_Flat_Obj_Buffers ( D3D11_USAGE_DEFAULT, 1, 6, nullptr );

	ResManager->SetActiveShadersInProgramm ( Global_text->TextShaderIndex );
	// Наносим текстуру с текстом на нужною нам текстуру
	CopyTextTextureOnTexture ( textureForDraw, 
		ResManager->Get_Flat_ObjectBuffers_ByIndex ( Res1 ),
		ResManager->Get_Flat_ObjectBuffers_ByIndex ( Res2 ) );
	//	CopyTextTextureOnTexture_OLD ( textureForDraw );

	ResManager->Delete_Flat_ObjectBuffers ( Res1 );
	ResManager->Delete_Flat_ObjectBuffers ( Res2 );
	
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


	// Копируем описание входной текстуры для скорости заполнения буфера CutTextureCopyForMapDesc
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
	CutTextureCopyForMap->Release ();
	InputTexture->Release ();
	MyRes->Release ();
	CutTexture->Release ();
	delete Box;
	delete CutTextureDesc;
	delete InputTextureDesc;
	delete CutTextureCopyForMapDesc;
	return true;
}



#include "stdafx.h"
///////////////////////////////////////////////////////////////////////////////
// Filename: fontclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "fontclass.h"

// http://stackoverflow.com/questions/12651597/directx-shader-resource-view-in-shaders

// Direct2D and Direct3D Interoperability Overview
// https://msdn.microsoft.com/en-us/library/windows/desktop/dd370966(v=vs.85).aspx#prerequisites
// http://xboxforums.create.msdn.com/forums/p/103939/615166.aspx

// +++++++++++++++++++++   ДЛЯ РИСОВАНИЯ ОБВОДКИ ТЕКСТА   ++++++++++++++++++++++
// https://msdn.microsoft.com/en-us/library/windows/desktop/dd941787%28v=vs.85%29.aspx#using_the_custom_text_renderer
// 1) Create the path geometry.
// 2) Write to the path geometry using the geometry sink.
// 3) Get the glyph run outline geometries back from DirectWrite and place them within the geometry sink.
// ---------------------   ДЛЯ РИСОВАНИЯ ОБВОДКИ ТЕКСТА   ----------------------

FontClass::FontClass() : SymbolAmount_def( 160 ) , fontsize_def ( 36.0f ) , color_step ( 0.1f )
{
		   Result = false;
		 FontName = new WCHAR [64];
			  Str = new WCHAR [640];

wcscpy_s(Str, 640, L"! \" # $ % & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ \\ ] ^ _ ` a b c d e f g h i j k l m n o p q r s t u v w x y z { | } ~ А Б В Г Д Е Ж З И Й К Л М Н О П Р С Т У Ф Х Ц Ч Ш Щ Ъ Ы Ь Э Ю Я а б в г д е ж з и й к л м н о п р с т у ф х ц ч ш щ ъ ы ь э ю я Ё ё ");

	 SymbolAmount = SymbolAmount_def; // чтобы занять нулевой символ
RealAmontInString = SymbolAmount * 2; // включая пробелы после каждой буквы
		 FontSize = fontsize_def;	
FontHeightInPixel = 0;

// Объявляем массив данных каждого символа формируемый при сканировании текстуры
	      Symbols = new _Symbol_Dim[SymbolAmount];
ZeroMemory(Symbols, sizeof(_Symbol_Dim) * SymbolAmount);

			  Top = 0;
			 Left = 0;
		   Bottom = 0;
		    Right = 0;
	FTextureWidth = 0;
   FTextureHeight = 0;

		d3d11Device = nullptr;
		d3d11DevCon = nullptr;
	  DWriteFactory = nullptr;
	D2DRenderTarget = nullptr;
		sharedTex11 = nullptr;
 Created_textureBuf = nullptr;
		FontTexture = nullptr;
		 d2dTexture = nullptr;
	  FontShaderRes = nullptr;
		 D2DFactory = nullptr;
}


FontClass::FontClass(const FontClass& other) : SymbolAmount_def( 160 ) , fontsize_def ( 36.0f ) , color_step ( 0.1f )
{
}


FontClass::~FontClass()
{
	delete [] Str;
	delete [] FontName;
	delete [] Symbols;
}

bool FontClass::Initialize(D3DGlobalContext* D3DGC, FontOnTextureData *FOTData1 )
{
	bool result = true;
	
	bool SaveFont = false;		// Если нужно сохранить текстуру шрифта
	bool SaveFontData = false;	// Если нужно сохранить данные к текстуре шрифта

	Local_D3DGC = D3DGC;

	d3d11Device = D3DGC->DX_device;

	keyedMutex11 = D3DGC->keyedMutex11;
	keyedMutex10 = D3DGC->keyedMutex10;
 D2DRenderTarget = D3DGC->D2DRenderTarget;
	 sharedTex11 = D3DGC->sharedTex11;
	  D2DFactory = D3DGC->D2DFactory;
Created_textureBuf = D3DGC->sharedTex11_MAPED;
	  d2dTexture = D3DGC->sharedTex11_SRV;
		   Width = D3DGC->sharedTex11_Width;
		  Height = D3DGC->sharedTex11_Height;

#if defined( DEBUG ) || defined( _DEBUG )
	DebugDevice = D3DGC->DebugDevice;
#endif

	d3d11DevCon = D3DGC->DX_deviceContext;
  DWriteFactory = D3DGC->DWriteFactory;


	if ( FOTData1->OutlineWidth > 10.0f )
		FOTData1->OutlineWidth = 10.0f;
	else if ( FOTData1->OutlineWidth < 0.0f )
		FOTData1->OutlineWidth = 0.0f;
// Если нет Outline, то и корректору указываем что не нужно бодавлять пиксели к буквам
	if ( FOTData1->Outline == false )
		FOTData1->OutlineWidth = 0.0f;
	
	if ( FOTData1->FontSize > 74.0f )
		FOTData1->FontSize = 74.0f;
	if ( FOTData1->FontSize < 0.0f )
		FOTData1->FontSize = 8.0f;

	FontSize = FOTData1->FontSize;

	CreateTextureFontFromFontFile( FOTData1->FontName, FOTData1, false, false );
	// Определяем размеры получившейся текстуры шрифта 
	CalcFTextureDimention( PackHelper( 0.0f, 0.0f, 0.0f, 0.0f ) );
	
	// Вносим коррекцию на Outline
	CorrectSymbolsDim( FOTData1->OutlineWidth , FTextureWidth );

	D3D11_TEXTURE2D_DESC sharedTexDesc;

	ZeroMemory( &sharedTexDesc, sizeof( sharedTexDesc ) );

	// Сохраняем высоту шрифта в пикселях
	FontHeightInPixel = FTextureHeight;

	sharedTexDesc.Width = FTextureWidth;
	sharedTexDesc.Height = FTextureHeight;
	sharedTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sharedTexDesc.MipLevels = 1;
	sharedTexDesc.ArraySize = 1;
	sharedTexDesc.SampleDesc.Count = 1;
	sharedTexDesc.Usage = D3D11_USAGE_DEFAULT; //D3D11_USAGE_IMMUTABLE;
	sharedTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	sharedTexDesc.MiscFlags = 0;
	hr = d3d11Device->CreateTexture2D( &sharedTexDesc, NULL, &FontTexture );
	if ( FAILED( hr ) )
	{
		MessageBox( D3DGC->hwnd, L"Could not initialize FontTexture.", Error, MB_OK );
		result = false;
		Shutdown();
		goto END;
	}
	
	// Копируем фонт в созданную текстуру FontTexture из которой будет производиться отрисовка символов по координатам
	D3D11_BOX* Box = new D3D11_BOX;
	ZeroMemory( Box, sizeof( D3D11_BOX ) );

	Box->top = Top;
	Box->left = 0;//Left;  Не используем Left потому, что мы не хотим обрезать текстуру фонта слева
	Box->bottom = Bottom;
	Box->right = Right;
	Box->front = 0;
	Box->back = 1;

	ID3D11Resource* tempResource;
	//  Получаем текстуру на которой мы рисовали текст, для того чтобы вырезать кусок содержащий ттекст
	d2dTexture->GetResource( &tempResource );
	// Вырезаем кусок с текстом и рисекм им на тесктуре
	d3d11DevCon->CopySubresourceRegion( FontTexture, 0, 0, 0, 0, tempResource, 0, Box );
	// GetResource нужно делать Release
	tempResource->Release();
//	d3d11DevCon->CopyResource(FontTexture, D3DGC->sharedTex11);
	
	delete Box;
		
	hr = d3d11Device->CreateShaderResourceView(FontTexture, NULL, &FontShaderRes);
	if ( FAILED( hr ) )
	{
		MessageBox( D3DGC->hwnd, L"Could not create ShaderResourceView FontTexture.", Error, MB_OK );
		Shutdown();
		result = false;
	}

	if ( SaveFont )
	{

		ID3D11Texture2D* SavetoFileFontTexture;

		ZeroMemory( &sharedTexDesc, sizeof( sharedTexDesc ) );

		sharedTexDesc.Width = FTextureWidth;
		sharedTexDesc.Height = FTextureHeight;
		sharedTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sharedTexDesc.MipLevels = 1;
		sharedTexDesc.ArraySize = 1;
		sharedTexDesc.SampleDesc.Count = 1;
		sharedTexDesc.BindFlags = 0;
		sharedTexDesc.MiscFlags = 0;
		sharedTexDesc.Usage = D3D11_USAGE_STAGING;
		sharedTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		hr = d3d11Device->CreateTexture2D( &sharedTexDesc, NULL, &SavetoFileFontTexture );

		// Если нужно сохранять ШРИФТ НА ТЕКСТУРЕ в ФАЙЛ
		d3d11DevCon->CopyResource( SavetoFileFontTexture, FontTexture );

		D3D11_MAPPED_SUBRESOURCE  mapResource;
		hr = d3d11DevCon->Map( SavetoFileFontTexture, 0, D3D11_MAP_READ, NULL, &mapResource );
		if ( !FAILED( hr ) )
		{
			union _block
			{
				byte  *data;
				unsigned int *udata;
			} block;

			int amount = mapResource.DepthPitch; // RowPitch * Height;
			int ui_amount = mapResource.DepthPitch / 4; // количество пикселей из 4 байт
			block.data = new byte[amount];

			Image img;
			D3D11_TEXTURE2D_DESC* TextureDesc = new D3D11_TEXTURE2D_DESC;

			memcpy( block.data, mapResource.pData, amount );
			FontTexture->GetDesc( TextureDesc );
			img.height = TextureDesc->Height;
			img.width = TextureDesc->Width;
			img.format = TextureDesc->Format;
			img.rowPitch = mapResource.RowPitch;
			img.slicePitch = mapResource.DepthPitch;
			img.pixels = block.data;

			hr = SaveToWICFile( img, WIC_FLAGS_NONE, GetWICCodec( WIC_CODEC_PNG ), L"Font.png" );
			if ( FAILED( hr ) )
			{
				MessageBox( hwnd, L"Could not save font.PNG file.", Error, MB_OK );
			}

			delete TextureDesc;
			delete[] block.data;
		}
		d3d11DevCon->Unmap( SavetoFileFontTexture, 0 );
		SavetoFileFontTexture->Release();
	}

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Отремить если нужно сохранить файл с координатами символов в текстуре
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if ( SaveFontData )
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


END:
	return result;

}


void FontClass::Shutdown()
{
	RCUBE_RELEASE( FontShaderRes );
	RCUBE_RELEASE( FontTexture );
/*
#if defined( DEBUG ) || defined( _DEBUG )
	DebugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif
*/
	return;
}


// GetTexture returns the font texture interface so the font graphics can be rendered.

ID3D11ShaderResourceView* FontClass::GetTexture()
{
	return FontShaderRes;
}

// BuildVertexArray will be called by the TextClass to build vertex buffers out of the text sentences it sends to this function as input. This way each sentence in the TextClass that needs
// to be drawn has its own vertex buffer that can be rendered easily after being created. The vertices input is the pointer to the vertex array that will be returned to the TextClass once it is 
// built. The sentence input is the text sentence that will be used to create the vertex array. The drawX and drawY input variables are the screen coordinates of where to draw the sentence.

void FontClass::BuildVertexArray(void* vertices, char* sentence, float& drawX, float& drawY, float& RenderFontSize)
{
	VertexType* vertexPtr;
	int numLetters, index, i, letter;


	// Coerce the input vertices into a VertexType structure.
	vertexPtr = (VertexType*)vertices;

	// Get the number of letters in the sentence.
	numLetters = (int)strlen(sentence);

	// Initialize the index to the vertex array.
	index = 0;

// The following loop will now build the vertex and index arrays. It takes each character from the sentence and creates two triangles for it. It then maps the character from the font texture onto 
// those two triangles using the m_Font array which has the TU texture coordinates and pixel size. Once the polygon for that character has been created it then updates the X coordinate on the 
// screen of where to draw the next character.

	// Draw each letter onto a quad.
	for(i=0; i<numLetters; ++i)
	{
//  +++++++++   Добавлена поддержка ASCII символов после 127   +++++++++++
		char *Source = &sentence[i];

		if ( *Source < 0 )
			letter = ((int) *Source ) + 159; // 191 - 32
		else
			letter = ((int) *Source ) - 32;
		// Беква Ё
		if ( *Source == -88 )
			letter = 159;
		// Бука ё
		else if ( *Source == -72 )
			letter = 160;

//  --------   Добавлена поддержка ASCII символов после 127   -----------

		// If the letter is a space then just move over three pixels.
		if(letter == 0)
		{
			// Ширина пробела
			drawX += FontSize / 2;
		}
		else
		{
			_Symbol_Dim *Source = &Symbols[letter - 1];
			float DYRFS = drawY - RenderFontSize;
			float DXSS = drawX + Source->Symbol_Width;
			// First triangle in quad.
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = XMFLOAT2( Source->Start, 0.0f);
			++index;

			vertexPtr[index].position = XMFLOAT3((drawX + Source->Symbol_Width), DYRFS, 0.0f);  // Bottom right.
			vertexPtr[index].texture = XMFLOAT2( Source->End, 1.0f);
			++index;

			vertexPtr[index].position = XMFLOAT3(drawX, DYRFS, 0.0f);  // Bottom left.
			vertexPtr[index].texture = XMFLOAT2( Source->Start, 1.0f);
			++index;

			// Second triangle in quad.
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = XMFLOAT2( Source->Start, 0.0f);
			++index;

			vertexPtr[index].position = XMFLOAT3( DXSS, drawY, 0.0f);  // Top right.
			vertexPtr[index].texture = XMFLOAT2( Source->End, 0.0f);
			++index;

			vertexPtr[index].position = XMFLOAT3(DXSS, DYRFS, 0.0f);  // Bottom right.
			vertexPtr[index].texture = XMFLOAT2( Source->End, 1.0f);
			++index;

			// Update the x location for drawing by the size of the letter and one pixel.
			drawX = drawX + Source->Symbol_Width + 1.0f;
		}
	}

	return;
}
/*
void FontClass::SetTexture(ID3D11ShaderResourceView* Texture)
{
	RCUBE_RELEASE(FontShaderRes);
	FontShaderRes = Texture;
}
*/


float FontClass::Round ( float Val )
{
	float Temp;
	// Почему-то нарушаются шрифты, если это убрать
	if ( Val > 228.0f )
		Temp = Val;
	else
		Temp = Val > 0.0f ? Val + 0.5f : Val - 0.5f;
	return floor ( Temp );
}


float FontClass::RoundToByte ( float Val )
{
	float Temp;
	
	Temp = Val > 0.0f ? Val + 0.5f : Val - 0.5f;
	
	return floor ( Temp );
}


float FontClass::ClampAndRound(float value, float min, float max)
	{
			if (value <= min)
			{
				return min;
			}
			if (value >= max)
			{
				return max;
			}
			return Round( value );
	}

UINT FontClass::PackUNorm(float bitmask, float value)
{
        value *= bitmask;
        return (unsigned int)ClampAndRound(value, 0.0f, bitmask);
}

UINT FontClass::PackHelper(float vectorX, float vectorY, float vectorZ, float vectorW)
{
        unsigned int  num = PackUNorm(255.0f, vectorX);
        unsigned int num2 = PackUNorm(255.0f, vectorY) << 8;
        unsigned int num3 = PackUNorm(255.0f, vectorZ) << 16;
        unsigned int num4 = PackUNorm(255.0f, vectorW) << 24;
        return num | num2 | num3 | num4;
}

UINT FontClass::R8G8B8A8toB8G8R8A8( UINT Value )
{
/*		UINT Amask = 0xFF000000;
		UINT Bmask = 0x00FF0000;
		UINT Gmask = 0x0000FF00;
		UINT Rmask = 0x000000FF;
*/
		UINT  numB = ( Value & 0x00FF0000 ) >> 16;
		UINT  numG = ( Value & 0x0000FF00 );
		UINT  numR = ( Value & 0x000000FF ) << 16;
		UINT  numA = ( Value & 0xFF000000 );

		return numB | numG | numR | numA;
}


void FontClass::CalcFTextureDimention( UINT FonColour )
{
	d3d11DevCon->CopyResource(Created_textureBuf,sharedTex11);

	D3D11_MAPPED_SUBRESOURCE  mapResource;

    hr = d3d11DevCon->Map(Created_textureBuf, 0, D3D11_MAP_READ, NULL, &mapResource);
	if(!FAILED(hr))
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
	memcpy(block.data, mapResource.pData, amount);

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
			if ( block.udata[i] != 0 && block.udata[i] != FonColour )
			{
				if ( FirstPoint == true )
				{
					FirstPoint = false;
					Top = y;
					Left = x;
				}

				if ( x < Left )
				{
					Left = x;
				}
				if ( x > Right )
				{
					Right = x+1;
				}
				if ( y > Bottom )
				{
					Bottom = y+1;
				}
			}
			++x;
// счётчик строк и столбцов пикселей в текстуре
// изменяем координаты следующего пикселя в текстуре
			if ( x == Columns )
			{
				++y;
				x=0;
			}
		}
	   FTextureWidth = ++Right; // Почему-то нужно прибавить 1 ( у буквы ё обрезался хвостик )
	   FTextureHeight = ++Bottom - Top ;
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
		   MessageBox(hwnd, L"Could not save font.PNG file.", Error, MB_OK);
	   }
*/

	   delete[] block.data;
	}
	d3d11DevCon->Unmap(Created_textureBuf,0);

}	


UINT FontClass::GetFontHeight()
{
	return Height;
}


int FontClass::GetFontHeightInPixels()
{
	return FontHeightInPixel;
}


void FontClass::GCharWidth(wchar_t c)
{
/*
	char* szOutline = "Colins !";

IDWriteFontFile*        g_pFontFile         = NULL;


    // Create font file reference
	WCHAR* Pref =  L"C:\\Windows\\Fonts\\times.ttf";
	UINT Length = (UINT)(wcslen(Pref) + TextFormat->GetFontFamilyNameLength());
    WCHAR* filePath = new WCHAR[Length]; // L"C:\\Windows\\Fonts\\RupsterScriptFree.ttf";
	WCHAR* FontName = new WCHAR[TextFormat->GetFontFamilyNameLength()];
	TextFormat->GetFontFamilyName(FontName,TextFormat->GetFontFamilyNameLength());

	wcscpy_s(filePath, Length, Pref);
	wcscat_s(filePath, Length, FontName);

    hr = DWriteFactory->CreateFontFileReference( filePath, NULL, &g_pFontFile );
    if(FAILED(hr))
    {
        MessageBox(NULL, L"Create font file reference failed!", Error, 0);
        return;
    }

	delete [] filePath;
	delete [] FontName;
    // Create font face
    IDWriteFontFile* fontFileArray[] = { g_pFontFile };

	hr = DWriteFactory->CreateFontFace( DWRITE_FONT_FACE_TYPE_TRUETYPE, 1, fontFileArray, 0, DWRITE_FONT_SIMULATIONS_NONE, &FontFace);
    if(FAILED(hr))
    {
        MessageBox(NULL, L"Create font file face failed!", Error, 0);
        return;
    }

	strlen (szOutline);
	UINT* pCodePoints = new UINT[strlen(szOutline)];
	UINT16* pGlyphIndices = new UINT16[strlen(szOutline)];
	ZeroMemory(pCodePoints, sizeof(UINT) * strlen(szOutline));
	ZeroMemory(pGlyphIndices, sizeof(UINT16) * strlen(szOutline));
	for (int i = 0; i < strlen(szOutline); ++i)
	{
		pCodePoints[i] = szOutline[i];
	}
	FontFace->GetGlyphIndicesW(pCodePoints, (UINT)strlen(szOutline), pGlyphIndices);


	hr = FontFace->GetGlyphRunOutline(
		(36.0f / 72.0f)*96.0f , // 36.0f - размер шрифта
		pGlyphIndices,
		NULL,
		NULL,
		(UINT)strlen(szOutline),
		FALSE,
		FALSE,
		pSink);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"GetGlyphRunOutline failed!", Error, 0);
		return;
	}

	// Create an array of gradient stops to put in the gradient stop
	// collection that will be used in the gradient brush.
	ID2D1LinearGradientBrush* m_pLinearGradientBrush;
	ID2D1SolidColorBrush* m_pSolidBrushOutline;
	ID2D1GradientStopCollection* pGradientStops = NULL;

	D2D1_GRADIENT_STOP gradientStops[3];
	gradientStops[0].color = D2D1::ColorF(D2D1::ColorF::Blue, 1);
	gradientStops[0].position = 0.0f;
	gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::Purple, 1);
	gradientStops[1].position = 0.5f;
	gradientStops[2].color = D2D1::ColorF(D2D1::ColorF::Red, 1);
	gradientStops[2].position = 1.0f;

	D2DRenderTarget->CreateGradientStopCollection(
		gradientStops,
		3,
		D2D1_GAMMA_2_2,
		D2D1_EXTEND_MODE_CLAMP,
		&pGradientStops
		);

	D2DRenderTarget->CreateLinearGradientBrush(
		D2D1::LinearGradientBrushProperties(
			D2D1::Point2F(0.0, -30.0),
			D2D1::Point2F(0.0, 0.0)),
		pGradientStops,
		&m_pLinearGradientBrush
		);

	D2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Plum),
		&m_pSolidBrushOutline
		);


	D2DRenderTarget->BeginDraw();
	// First make sure the transformation is set to the identity transformation.
	D2DRenderTarget->SetTransform(D2D1::IdentityMatrix());
	// Clear the background of the window with a white color.
	D2DRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	// shift the text down
	D2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(10.0f, 60.0f));
	// Draw text outline
	D2DRenderTarget->DrawGeometry(pPathGeometry, m_pSolidBrushOutline, 3.0f);
	// Draw text body
	D2DRenderTarget->FillGeometry(pPathGeometry, m_pLinearGradientBrush);

	HRESULT hr = D2DRenderTarget->EndDraw();


	pGradientStops->Release();
	m_pSolidBrushOutline->Release();
	pGradientStops->Release();


	hr = pSink->Close();
	if (FAILED(hr))
	{
		MessageBox(NULL, L"pSink close failed!", Error, 0);
		return;
	}



	if (pCodePoints)
	{
		delete[] pCodePoints;
		pCodePoints = NULL;
	}

	if (pGlyphIndices)
	{
		delete[] pGlyphIndices;
		pGlyphIndices = NULL;
	}


*/


/*
    wchar_t textString[] = {c, '\0'};

    // Get text length
    UINT32 textLength = (UINT32)wcslen(textString);

    UINT32* pCodePoints     = new UINT32[textLength];
    ZeroMemory(pCodePoints, sizeof(UINT32) * textLength);

    UINT16* pGlyphIndices   = new UINT16[textLength];
    ZeroMemory(pGlyphIndices, sizeof(UINT16) * textLength);

    for(unsigned int i = 0; i < textLength; ++i)
    {
        pCodePoints[i] = textString[i];
    }

    // Get glyph indices
    hr = FontFace->GetGlyphIndices(
        pCodePoints,
        textLength,
        pGlyphIndices
        );
    if(FAILED(hr))
    {
        MessageBox(NULL, L"Get glyph indices failed!", Error, 0);
        return;
    }

    DWRITE_GLYPH_METRICS* glyphmetrics = new DWRITE_GLYPH_METRICS[textLength];
	FontFace->GetDesignGlyphMetrics(pGlyphIndices, textLength, glyphmetrics);


	DWRITE_FONT_METRICS * FontMetrix = new DWRITE_FONT_METRICS[textLength];
	FontFace->GetMetrics(FontMetrix);
*/

//	delete [] pCodePoints;
//	delete [] pGlyphIndices;
//	delete [] FontMetrix;
//   delete [] glyphmetrics;

}


// http://stackoverflow.com/questions/726549/algorithm-for-additive-color-mixing-for-rgb-values
UINT FontClass::ColourMixing ( UINT Background, UINT Foreground )
{

	union _MyData
	{
		_MyData ()
		{
		}; //MyData.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };
		XMFLOAT3 Fl3;
		XMFLOAT4 Fl4;
		XMVECTOR Vec;
	} Fore, Back;

	union _block
	{
		byte  *data;
		unsigned int *udata;
	} UFore, UBack, UResColor;

	UINT InFore = Foreground;
	UINT InBack = Background;
	UINT ResultColor = 0;
	
	UFore.udata = &InFore;
	UBack.udata = &InBack;

	float One = 0.00392156; //1.0f / 255.0f;

	Fore.Fl4 = {( float ) UFore.data[0],
				( float ) UFore.data[1],
				( float ) UFore.data[2],
				( float ) UFore.data[3]};

	Fore.Vec *= One;

	Back.Fl4 = {( float ) UBack.data[0],
				( float ) UBack.data[1],
				( float ) UBack.data[2],
				( float ) UBack.data[3]};

	Back.Vec *= One;

	float A,R,G,B,Alpha;

		Alpha = 1.0f - Fore.Fl4.w;
		A = Fore.Fl4.w + Back.Fl4.w * Alpha;

	if ( A == 0.0f )
		return InBack;

	B = ( Fore.Fl4.x * Fore.Fl4.w + Back.Fl4.x * Back.Fl4.w * Alpha ) / A;
	G = ( Fore.Fl4.y * Fore.Fl4.w + Back.Fl4.y * Back.Fl4.w * Alpha ) / A;
	R = ( Fore.Fl4.z * Fore.Fl4.w + Back.Fl4.z * Back.Fl4.w * Alpha ) / A;


	UResColor.udata = &ResultColor;

	UResColor.data[0] = byte ( RoundToByte ( R / One) );
	UResColor.data[1] = byte ( RoundToByte ( G / One) );
	UResColor.data[2] = byte ( RoundToByte ( B / One) );
	UResColor.data[3] = byte ( RoundToByte ( A / One) );

	return ResultColor;
/*
	var rgbaSum = function(c1, c2){
	var a = c1.a + c2.a*(1-c1.a);
	return {
	r: (c1.r * c1.a  + c2.r * c2.a * (1 - c1.a)) / a,
	g: (c1.g * c1.a  + c2.g * c2.a * (1 - c1.a)) / a,
	b: (c1.b * c1.a  + c2.b * c2.a * (1 - c1.a)) / a,
	a: a
	}
	}
*/
}


void FontClass::UINToXMFloat4 ( UINT &Value, XMFLOAT4 &Result )
{

}


bool FontClass::CreateTextureFontFromFontFile( WCHAR* FontNameFullPath, FontOnTextureData *FOTData, bool fontTextureOut, bool fontTextDescOut )
{

	bool result = true; // если всё классно, функция 

	IDWriteFontFile						   *g_pFontFile = nullptr;
	IDWriteFontFace							  *FontFace = nullptr;
//	ID2D1LinearGradientBrush	*m_pLinearGradientBrush = nullptr;
	ID2D1SolidColorBrush			   *SolidColorBrush = nullptr;
//	ID2D1SolidColorBrush		  *m_pSolidBrushOutline = nullptr;
//	ID2D1GradientStopCollection			*pGradientStops = nullptr;

	WCHAR *Str_COPY = new WCHAR[640];

	wcscpy_s( Str_COPY, 640, Str );


	hr = DWriteFactory->CreateFontFileReference( FontNameFullPath, NULL, &g_pFontFile );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create font file reference failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	IDWriteFontFile* fontFileArray[] = { g_pFontFile };

	hr = DWriteFactory->CreateFontFace( FOTData->FontFaceType, 1, fontFileArray, 0, DWRITE_FONT_SIMULATIONS_NONE, &FontFace );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create font file face failed!", Error, 0 );
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
	keyedMutex11->ReleaseSync( 0 );

	//Use D3D10.1 device
	keyedMutex10->AcquireSync( 0, 0 );

	D2DRenderTarget->BeginDraw();

	// First make sure the transformation is set to the identity transformation.
	// D2DRenderTarget->SetTransform ( D2D1::IdentityMatrix () );

	// Очищаем текстуру для рисования
	//	D2DRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	D2DRenderTarget->Clear( D2D1::ColorF( 0.0f, 0.0f, 0.0f, 0.0f ) );

	// Почему-то нужно сдвинуть текст вниз чтобы он разместился на текстуре
	D2DRenderTarget->SetTransform( D2D1::Matrix3x2F::Translation( 0.0f, FOTData->FontSize ) );  // 20 как раз для шрифта 36.0

	// Выключаем антиалиасинг, чтобы можно было распознать буквы по цвету
	D2DRenderTarget->SetAntialiasMode( D2D1_ANTIALIAS_MODE_ALIASED );

	UINT L = 0; // Счётчик символов
	D2D1_COLOR_F FontColor; // Цвет которым красим символы

	// Создаём цвет для заливки им символов
	D2DRenderTarget->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::Plum ), // просто указали произвольный цвет при создании
											&SolidColorBrush
											);

	for ( int i = 0; i < SymbolAmount; ++i )
	{

		if ( edin == 0 )
		{
			edin = 10;
			ten -= 1;
			R = 1.0f;
			G -= color_step;
		}
		if ( ten == 0 )
		{
			ten = 10;
			sot -= 1;
			G = 1.0f;
			B -= color_step;
		}
		if ( sot == 0 )
		{
			sot = 10;
			B = 1.0f;

		}

		//Set the Font Color
		FontColor = D2D1::ColorF( R, G, B, 1.0f );

		// Сохраняем цвет в массив описывающий символ, чтобы потом можно было его найти по цвету в текстуре
		Symbols[SymbolAmount - 1 - i].Color = PackHelper( R, G, B, 1.0f );
		
		// Ставим цвет которым будем рисовать символ
		SolidColorBrush->SetColor( FontColor );


// +++++++++++++++++++++++    РИСУЕМ ТЕКСТ    ++++++++++++++++++++++++++++++++++++++++++++
		ID2D1PathGeometry					 *pPathGeometry = nullptr;
		ID2D1GeometrySink							 *pSink = nullptr;
		UINT								   *pCodePoints = nullptr;
		UINT16								 *pGlyphIndices = nullptr;

		L = ( UINT ) wcslen( Str_COPY ); // Строка текста содержащая все нужные символы

		pCodePoints = new UINT[L];
		pGlyphIndices = new UINT16[L];

		ZeroMemory( pCodePoints, sizeof( UINT ) * L );
		ZeroMemory( pGlyphIndices, sizeof( UINT16 ) * L );

		//	memcpy( pCodePoints, text, sizeof ( UINT ) * L );

		UINT *Dest = &pCodePoints[0];
		wchar_t *Source = &Str[0];
		for ( UINT i = 0; i < L; ++i )
		{
			//		pCodePoints[i] = text[i];
			*Dest = *Source;
			++Dest;
			++Source;
		}

		hr = FontFace->GetGlyphIndicesW( pCodePoints, L, pGlyphIndices );
		if ( FAILED( hr ) )
		{
			MessageBox( NULL, L"Create GetGlyphIndices failed!", Error, 0 );
			result = false;
//			goto EXIT;
		}

		hr = D2DFactory->CreatePathGeometry( &pPathGeometry );
		if ( FAILED( hr ) )
		{
			MessageBox( NULL, L"Create CreatePathGeometry failed!", Error, 0 );
			result = false;
//			goto EXIT;
		}

		// Write to the path geometry using the geometry sink.
		hr = pPathGeometry->Open( &pSink );
		if ( FAILED( hr ) )
		{
			MessageBox( NULL, L"Open pPathGeometry failed!", Error, 0 );
			result = false;
//			goto EXIT;
		}

		hr = FontFace->GetGlyphRunOutline(
			FOTData->FontSize,//( 20.0f / 72.0f )*96.0f, // 36.0f - размер шрифта
			pGlyphIndices,
			NULL,
			NULL,
			L,
			FALSE,
			FALSE,
			pSink );
		if ( FAILED( hr ) )
		{
			MessageBox( NULL, L"GetGlyphRunOutline failed!", Error, 0 );
			result = false;
			//		goto EXIT;
		}

		hr = pSink->Close();
		if ( FAILED( hr ) )
		{
			MessageBox( NULL, L"pSink close failed!", Error, 0 );
			result = false;
			//		goto EXIT;
		}

		// Draw text body
		D2DRenderTarget->FillGeometry( pPathGeometry, SolidColorBrush );


		edin -= 1;
		R -= color_step;

		// Уменьшаем строку на один символ
		if ( L > 2 )
			Str_COPY [ L-2 ] = NULL;

		RCUBE_ARR_DELETE( pCodePoints );
		RCUBE_ARR_DELETE( pGlyphIndices );
		RCUBE_RELEASE( pPathGeometry );
		RCUBE_RELEASE( pSink );

	} // for

	delete [] Str_COPY;

// -------------------   КОНЕЦ рисования букв разным цветом   -------------------------------

	hr = D2DRenderTarget->EndDraw();
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"D2DRenderTarget->EndDraw() failed!", Error, 0 );
		result = false;
		//		goto EXIT;
	}

	//Release the D3D10.1 Device
	keyedMutex10->ReleaseSync( 1 );

	//Use the D3D11 Device
	keyedMutex11->AcquireSync( 1, 0 );

	// -----------------------------------------------------------
	// Если нужно сохранять цветную картинку
	d3d11DevCon->CopyResource( Created_textureBuf, sharedTex11 );

	D3D11_MAPPED_SUBRESOURCE  mapResource;

	hr = d3d11DevCon->Map( Created_textureBuf, 0, D3D11_MAP_READ, NULL, &mapResource );
	if ( !FAILED( hr ) )
	{
		union _block
		{
			byte  *data;
			unsigned int *udata;
		} block;

		int amount = mapResource.DepthPitch; // RowPitch * Height;
		int ui_amount = mapResource.DepthPitch / 4; // количество пикселей из 4 байт
		block.data = new byte[amount];
		// инициализируем массив
		memcpy( block.data, mapResource.pData, amount ); //mapResource.RowPitch * Height);
/*														 
// +++++++++++++++++++++++++   Сохраняем цветной шрифт   +++++++++++++++++++++++++++++
		Image img;
		D3D11_TEXTURE2D_DESC* TextureDesc = new D3D11_TEXTURE2D_DESC;
		sharedTex11->GetDesc( TextureDesc );
		img.height = TextureDesc->Height;
		img.width = TextureDesc->Width;
		img.format = TextureDesc->Format;
		img.rowPitch = mapResource.RowPitch;
		img.slicePitch = mapResource.DepthPitch;
		img.pixels = block.data;

		hr = SaveToWICFile( img, WIC_FLAGS_NONE, GetWICCodec( WIC_CODEC_PNG ), L"ColourFont.png" );
		if ( FAILED( hr ) )
		{
			MessageBox( hwnd, L"Could not save font.PNG file.", Error, MB_OK );
		}
		delete TextureDesc;
// -------------------------   Сохраняем цветной шрифт   ----------------------------
*/   
// заполняем в массиве данных символов FirstPoint = true
		for ( int i = 0; i < SymbolAmount; ++i )
		{
			Symbols[i].FirstPoint = true;
		}

		// Размеры текстуры нужны чтобы правильно определять положение символа на текстуре
		int Columns = ui_amount / Height;
		int x = 0;
		int y = 0;
		int ActiveSymbol = 0;
		// сколько занимает один пиксель в текстуре

		double OnePixelLength = 1 / ( double ) ( Width ); // Берём ширину текстуры которая будет сохраняться в файл
		OnePixelLength *= 1000000000;
		OnePixelLength = floor( OnePixelLength );
		OnePixelLength /= 1000000000;

		float OnePixL = ( float ) OnePixelLength;

		for ( int t = 0; t < ui_amount; ++t )
		{

			if ( block.udata[t] != 0 )
			{
				unsigned int color = block.udata[t];
				for ( int i = 0; i < SymbolAmount; ++i )
				{
					if ( Symbols[i].Color == color )
					{
						ActiveSymbol = i;
						break;
					}
				}

				_Symbol_Dim *Dest = &Symbols[ActiveSymbol];

				// счётчик пикселей в символе
				++Dest->PointsAmount;
				// запоминаем самый левый пиксель в символе как самый левый и самый правый
				if ( Dest->FirstPoint )
				{
					// самый левый пиксель
					Dest->Start_Pixel_XPos = x; // символ начинается с 0 по X ( в любом случае )
					Dest->Start_Pixel_YPos = y;
					// самый правый пиксель
					//				 Symbols[0].End_Pixel_XPos = x;
					//				 Symbols[0].End_Pixel_YPos = y;
					Dest->FirstPoint = false;
				}

				if ( x < Dest->Start_Pixel_XPos )
					Dest->Start_Pixel_XPos = x;

				if ( x > Dest->End_Pixel_XPos )
					Dest->End_Pixel_XPos = x;
				if ( y > Dest->End_Pixel_YPos )
					Dest->End_Pixel_YPos = y;

			}
			++x;
			// счётчик строк и столбцов пикселей в текстуре
			// изменяем координаты следующего пикселя в текстуре
			if ( x > Columns - 1 )
			{
				++y;
				x = 0;
			}

			_Symbol_Dim *Dest = &Symbols[ActiveSymbol];

			Dest->Symbol_Heigth = ( float ) Dest->End_Pixel_YPos - Dest->Start_Pixel_YPos; // +1
			Dest->Symbol_Width = ( float ) Dest->End_Pixel_XPos - Dest->Start_Pixel_XPos; // +1 

			Dest->Start = Dest->Start_Pixel_XPos * OnePixL;
			Dest->End = ( Dest->End_Pixel_XPos + 1 ) * OnePixL;
		}

		delete[] block.data;
	}
	d3d11DevCon->Unmap( Created_textureBuf, 0 );

// +++++++++++++++++++     Теперь уже рисуем полноценно с OUTLINE, если нужно     ++++++++++++++++++++++++++++++++++++++++

	ID2D1PathGeometry					 *pPathGeometry = nullptr;
	ID2D1GeometrySink							 *pSink = nullptr;
	UINT								   *pCodePoints = nullptr;
	UINT16								 *pGlyphIndices = nullptr;

	L = ( UINT ) wcslen( Str );

	pCodePoints = new UINT[L];
	pGlyphIndices = new UINT16[L];

	ZeroMemory( pCodePoints, sizeof( UINT ) * L );
	ZeroMemory( pGlyphIndices, sizeof( UINT16 ) * L );

	//	memcpy( pCodePoints, text, sizeof ( UINT ) * L );

	UINT *Dest = &pCodePoints[0];
	wchar_t *Source = &Str[0];
	for ( UINT i = 0; i < L; ++i )
	{
		//		pCodePoints[i] = text[i];
		*Dest = *Source;
		++Dest;
		++Source;
	}

	hr = FontFace->GetGlyphIndicesW( pCodePoints, L, pGlyphIndices );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create GetGlyphIndices failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	hr = D2DFactory->CreatePathGeometry( &pPathGeometry );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create CreatePathGeometry failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	// Write to the path geometry using the geometry sink.
	hr = pPathGeometry->Open( &pSink );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Open pPathGeometry failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	hr = FontFace->GetGlyphRunOutline(
		FOTData->FontSize,//( 20.0f / 72.0f )*96.0f, // 36.0f - размер шрифта
		pGlyphIndices,
		NULL,
		NULL,
		L,
		FALSE,
		FALSE,
		pSink );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"GetGlyphRunOutline failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	hr = pSink->Close();
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"pSink close failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	D2DRenderTarget->SetAntialiasMode( FOTData->AliasingMode );

	//Release the D3D 11 Device
	keyedMutex11->ReleaseSync( 0 );

	//Use D3D10.1 device
	keyedMutex10->AcquireSync( 0, 0 );

	D2DRenderTarget->BeginDraw();

	// First make sure the transformation is set to the identity transformation.
	//	D2DRenderTarget->SetTransform ( D2D1::IdentityMatrix () );

	// Clear the background of the window with a white color.
	//	D2DRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	D2DRenderTarget->Clear( D2D1::ColorF( 0.0f, 0.0f, 0.0f, 0.0f ) );

	// shift the text down
	D2DRenderTarget->SetTransform( D2D1::Matrix3x2F::Translation( 0.0f, FOTData->FontSize ) );

	if ( FOTData->Outline )
	{
		// Draw text outline
		D2DRenderTarget->DrawGeometry( pPathGeometry, FOTData->OutlineBrush, FOTData->OutlineWidth );
	}

	// Draw text body
	D2DRenderTarget->FillGeometry( pPathGeometry, FOTData->FillBrush );

	hr = D2DRenderTarget->EndDraw();
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"D2DRenderTarget->EndDraw() failed!", Error, 0 );
		result = false;
		goto EXIT;
	}

	//Release the D3D10.1 Device
	keyedMutex10->ReleaseSync( 1 );

	//Use the D3D11 Device
	keyedMutex11->AcquireSync( 1, 0 );

	RCUBE_ARR_DELETE( pCodePoints );
	RCUBE_ARR_DELETE( pGlyphIndices );
	RCUBE_RELEASE( pPathGeometry );
	RCUBE_RELEASE( pSink );
/*
	if ( fontTextureOut )
	{
		// Если нужно сохранять ШРИФТ НА ТЕКСТУРЕ в ФАЙЛ
		d3d11DevCon->CopyResource( Created_textureBuf, sharedTex11 );

		hr = d3d11DevCon->Map( Created_textureBuf, 0, D3D11_MAP_READ, NULL, &mapResource );
		if ( !FAILED( hr ) )
		{
			union _block
			{
				byte  *data;
				unsigned int *udata;
			} block;

			int amount = mapResource.DepthPitch; // RowPitch * Height;
			int ui_amount = mapResource.DepthPitch / 4; // количество пикселей из 4 байт
			block.data = new byte[amount];

			Image img;
			D3D11_TEXTURE2D_DESC* TextureDesc = new D3D11_TEXTURE2D_DESC;

			memcpy( block.data, mapResource.pData, amount );
			sharedTex11->GetDesc( TextureDesc );
			img.height = TextureDesc->Height;
			img.width = TextureDesc->Width;
			img.format = TextureDesc->Format;
			img.rowPitch = mapResource.RowPitch;
			img.slicePitch = mapResource.DepthPitch;
			img.pixels = block.data;

			hr = SaveToWICFile( img, WIC_FLAGS_NONE, GetWICCodec( WIC_CODEC_PNG ), L"Font.png" );
			if ( FAILED( hr ) )
			{
				MessageBox( hwnd, L"Could not save font.PNG file.", Error, MB_OK );
			}

			delete TextureDesc;
			delete [] block.data;
		}
		d3d11DevCon->Unmap( Created_textureBuf, 0 );
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

//	RCUBE_RELEASE( m_pLinearGradientBrush );
//	RCUBE_RELEASE( m_pSolidBrushOutline );
	RCUBE_RELEASE( SolidColorBrush );
//	RCUBE_RELEASE( pGradientStops );
	RCUBE_RELEASE( FontFace );
	RCUBE_RELEASE( g_pFontFile );

	return result;
}


void FontClass::CorrectSymbolsDim( float OutlineSize, int NewTextureWidth )
{
	double OnePixelLength = 1 / ( double ) ( NewTextureWidth ); // Берём ширину текстуры которая будет сохраняться в файл
	OnePixelLength *= 1000000000;
	OnePixelLength = floor( OnePixelLength );
	OnePixelLength /= 1000000000;

	float OnePixL = ( float ) OnePixelLength;

	int Correction = (int)OutlineSize / 2;

	for ( int i = 0; i < SymbolAmount; ++i )
	{
		_Symbol_Dim *Dest = &Symbols[i];

		Dest->Start = ( Dest->Start_Pixel_XPos - Correction ) * OnePixL;
		Dest->End = ( Dest->End_Pixel_XPos + Correction + 1) * OnePixL;

	}
}
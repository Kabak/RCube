#include "stdafx.h"
#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
	RCube_Sentences.reserve ( 30 );
	ComeputeShaderNames.reserve(2);
	RCube_Font.reserve ( 10 );
	UnusedSentenceIndex.reserve ( 10 );
	UnusedFlatObjBuffersIndex.reserve ( 10 );
	UnusedTexturesIndex.reserve ( 10 );
}

ResourceManager::~ResourceManager()
{
	ShutDown();
}


void ResourceManager::ShutDown()
{
	size_t c = 0;
	size_t i = 0;
// CUBEMAPS
	c = 0;
	i = CubeMaps.size ();
	while ( c < i )
	{
		Delete_Flat_ObjectBuffers ( CubeMaps[c]->CubeMapBuffersIndex );
		DeleteTexture ( CubeMaps[c]->CubeMapTextureIndex );
		++c;
	}
	CubeMaps.clear ();
	
// PARTICLE SYSTEMS
/*
	c = 0;
	i = ParticleSystems.size ();
	while ( c < i )
	{
		DeleteParticleSystem ( c );
		++c;
	}
	ParticleSystems.clear ();
*/
	ParticleSystems.clear ();
	UnusedParticleSystemIndex.clear ();
	BB_ParticleSystems_Buffers.clear ();
	Unused_BB_BuffersIndex.clear ();

// TERRAIN
	c = 0;
	i = Terrains.size ();
	while ( c < i )
	{
		Delete_Terrain ( c );
		++c;
	}
	Terrains.clear ();
	TerrainVertexBuffers.clear ();
	UnusedTerrainsIndex.clear ();
	UnusedTerrainsBuffersIndex.clear ();

// 3D MODELS
	c = 0;
	i = _3DModels.size ();
	while ( c < i )
	{
		Delete_3D_Object( c );
		++c;
	}
	_3DModels.clear ();

	_3DModelsMeshBuffers.clear ();
	Unused3DModelsIndex.clear ();
	Unused3DModelsMeshBuffersIndex.clear ();

// SENTENCES
	DeleteAllSentences ();
	UnusedSentenceIndex.clear ();

// FONTS
	RCUBE_VECTOR_DELETE ( RCube_Font );
	UnusedFontIndex.clear ();			// Delete free font indexes

// TEXTURES
	RCUBE_VECTOR_DELETE ( TexturesArr );
	UnusedTexturesIndex.clear ();		// Delete free texture indexes

// VERTEX Shaders
	RCUBE_VECTOR_RELEASE ( VertShaderArr );

// PIXEL Shaders
	RCUBE_VECTOR_RELEASE ( PixShaderArr )

// BLOBS
	RCUBE_VECTOR_RELEASE ( BlobsArr );
	
// COMPUTE Shaders
	RCUBE_VECTOR_RELEASE ( ComeputeShaderArr );

// GEOMETRY Shaders
	RCUBE_VECTOR_RELEASE ( GeomShaderArr );

// HULL Shaders
	RCUBE_VECTOR_RELEASE (HullShderArr );

// DOMAIN Shaders
	RCUBE_VECTOR_RELEASE ( DomainShderArr );

// Compute Shaders Names
	RCUBE_VECTOR_ARR_DELETE ( ComeputeShaderNames );

// LAYOUTS
	RCUBE_VECTOR_RELEASE ( Layouts );

// SHADERS BUNCHES
	c = 0;
	while (c < BunchArr.size())
	{
		RCUBE_ARR_DELETE( BunchArr[c].BunchName );
		++c;
	}
	BunchArr.clear();

// Flat Objects
	RCUBE_VECTOR_DELETE ( FlatObjectBuffers );
	UnusedFlatObjBuffersIndex.clear ();	// Удаляем свободные индексы буферов

	Menus.clear ();					// Удаляем меню
}


void ResourceManager::RCUBE_ErrorMessageShow ( WCHAR* Prefix, WCHAR* Middle, WCHAR* Postfix )
{
	size_t PrefixLenght		= 0;
	size_t PostfixLenght	= 0;
	size_t MiddleLenght		= 0;

	Prefix  != nullptr ? PrefixLenght  = wcslen ( Prefix )  : PrefixLenght  = 0;
	Postfix != nullptr ? PostfixLenght = wcslen ( Postfix ) : PostfixLenght = 0;
	Middle  != nullptr ? MiddleLenght  = wcslen ( Middle )  : MiddleLenght  = 0;

	size_t Length = PrefixLenght + PostfixLenght + MiddleLenght + sizeof(WCHAR);

	wchar_t *Message = new wchar_t [ Length ];

	wcscat_s (Message, Length, Prefix);
	wcscat_s (Message, Length, Middle);
	wcscat_s (Message, Length, Postfix );

	MessageBox(0, Message, Error, MB_OK);

	delete [] Message;
}


int ResourceManager::GetNewTextureIndex ( Texture* NewTexture )
{
	int Index = -1;
	if (!UnusedTexturesIndex.empty ())
	{
		Index = UnusedTexturesIndex.back ();
		UnusedTexturesIndex.pop_back ();
		TexturesArr[Index] = NewTexture;
		TexturesArr[Index]->Index = Index;
	}
	else
	{
		TexturesArr.push_back ( NewTexture );
		Index = (int)(TexturesArr.size () - 1);
		TexturesArr[Index]->Index = Index;	// Сохраняем собственный индекс в своём теле
	}

	NewTexture->Index = Index;

	return  Index;
}


HRESULT ResourceManager::InitTextures(WCHAR * kafFilename){

	unsigned char * Buff = nullptr , * temp;
	decodeFile(&Buff, kafFilename  , 0);
	int DescStart;
	unsigned char IsKaf[4];
//	unsigned char IntBuff[4];
	unsigned char ObjFormat[3];
	int c = 0, readFileSize = 0;
	unsigned char* buffer;
	bool IsFormatIdetyfity;

	temp = Buff;
	memcpy(IsKaf, temp, 4);
	temp += 4;

	if (IsKaf[0] != ' ' && IsKaf[1] != 'K' && IsKaf[2] != 'A' && IsKaf[3] != 'F')
	{
		RCUBE_ErrorMessageShow ( nullptr, L"Texture .kaf file currupted.", InitTexture );
		return E_FAIL;
	}

	
	memcpy((unsigned char*)&DescStart, temp, sizeof(int));
	temp += sizeof(int);

	memcpy((unsigned char*)&TexturesNum, temp, sizeof(int));
	temp += sizeof(int);

	ID3D11ShaderResourceView * ShaderRes = nullptr;
	ID3D11Resource * LoadedTextureRes = nullptr;

	while (c < TexturesNum)
	{
		memcpy(ObjFormat, temp, 3);
		temp += 3;

		memcpy((unsigned char*)&readFileSize, temp, sizeof(int));
		temp += sizeof(int);

		buffer = new unsigned char[readFileSize];


		memcpy(&buffer[0], temp, readFileSize);
		temp += readFileSize;
		
		IsFormatIdetyfity = false;

		if ((ObjFormat[0] == 'j' && ObjFormat[1] == 'p' && ObjFormat[2] == 'g')
			|| (ObjFormat[0] == 'b' && ObjFormat[1] == 'm' && ObjFormat[2] == 'p')
			|| (ObjFormat[0] == 'p' && ObjFormat[1] == 'n' && ObjFormat[2] == 'g'))
		{

//			hr = DirectX::CreateWICTextureFromMemory(Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, &buffer[0], (size_t)readFileSize, &LoadedTextureRes
//			 , &ShaderRes, NULL);
// https://github.com/Microsoft/DirectXTK/wiki/WICTextureLoader
			hr = DirectX::CreateWICTextureFromMemoryEx( 
			Local_D3DGC->DX_device , 
			&buffer[0],
			( size_t ) readFileSize,
			0,
			D3D11_USAGE_DEFAULT,		// UsageFlags
			D3D11_BIND_SHADER_RESOURCE,	// BindFlags
			0,							// CPUFlags
			0,							// MiscFlags
			WIC_LOADER_FORCE_R8G8B8A8_UNORM,// #include "WICTextureLoader.h"
			&LoadedTextureRes,
			&ShaderRes
			);
		}
		else if ((ObjFormat[0] == 'd' && ObjFormat[1] == 'd' && ObjFormat[2] == 's'))
		{

			hr = DirectX::CreateDDSTextureFromMemory(Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, &buffer[0], (size_t)readFileSize, &LoadedTextureRes
				, &ShaderRes, NULL);
		}

		if ( FAILED( hr ) )
		{
			RCUBE_ErrorMessageShow ( Prefix, L"texture form .kaf file.", InitTexture );
			return E_FAIL;
		}
		else
		{
			D3D11_TEXTURE2D_DESC* InputTextureDesc = new D3D11_TEXTURE2D_DESC;
			ID3D11Texture2D* InputTexture;
			Texture* NewTexture = new Texture;

			LoadedTextureRes->QueryInterface<ID3D11Texture2D> ( &InputTexture );
			InputTexture->GetDesc ( InputTextureDesc );

			NewTexture->Width = InputTextureDesc->Width;
			NewTexture->Height = InputTextureDesc->Height;
			NewTexture->Resource = LoadedTextureRes;
			NewTexture->SRV = ShaderRes;
//			NewTexture->Type = _2D;
//			NewTexture->Format = InputTextureDesc->Format;
	
			RCUBE_RELEASE ( InputTexture );
			delete InputTextureDesc;

			ShaderRes ? NewTexture->ShaderResource = true : NewTexture->ShaderResource = false;

			GetNewTextureIndex ( NewTexture );

		}

		delete [] buffer;
		++c;
	}

	delete[] Buff;
	
	return S_OK;
}

HRESULT ResourceManager::InitShaders( WCHAR * kafFilename) {

	HRESULT Result = S_OK;

	unsigned char * Buff = nullptr, *temp;
	decodeFile(&Buff, kafFilename, 0);
	unsigned char IsKaf[4];
	unsigned char ObjFormat[3];
	int DescStart;	// адрес имён файлов
	int c = 0, readFileSize = 0;

	int VSCounter = 0;
	int CSCounter = 0;

	temp = Buff;
	memcpy(IsKaf, temp, 4);
	temp += 4;

	if (IsKaf[0] != ' ' && IsKaf[1] != 'K' && IsKaf[2] != 'A' && IsKaf[3] != 'F') 
	{
		RCUBE_ErrorMessageShow ( nullptr, L"Texture .kaf file currupted.", InitShader );
		return E_FAIL;
	}


	memcpy((unsigned char*)&DescStart, temp, sizeof(int));
	temp += sizeof(int);

	memcpy((unsigned char*)&ShadersNum, temp, sizeof(int));
	temp += sizeof(int);

	ID3D10Blob * Blob;
	ID3D11VertexShader *	m_vertexShader;
	ID3D11PixelShader *		m_pixelShader;
	ID3D11GeometryShader *	m_geomShader;
	ID3D11HullShader *		m_hullShader;
	ID3D11DomainShader *	m_domainShader;

	ID3D11ComputeShader *	m_ComputeShader;

	while (c < ShadersNum)
	{
		memcpy(ObjFormat, temp, 3);
		temp += 3;

		memcpy((unsigned char*)&readFileSize, temp, sizeof(int));
		temp += sizeof(int);

		hr = D3DCreateBlob(readFileSize, &Blob);
		if ( FAILED( hr ) )
		{
			RCUBE_ErrorMessageShow ( Prefix, L"BLOB.", InitShader );
			return hr;
		}

		memcpy(reinterpret_cast<unsigned char*>(Blob->GetBufferPointer()), temp, readFileSize);
		temp += readFileSize;

		BlobsArr.push_back(Blob);


		if (ObjFormat[0] == 'v' && ObjFormat[1] == 's' && ObjFormat[2] == ' ') 
		{
			// Create the vertex shader from the buffer.
			hr = Local_D3DGC->DX_device->CreateVertexShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &m_vertexShader);
			if (FAILED( hr ))
			{
				RCUBE_ErrorMessageShow ( Prefix, VS_str, InitShader );
				return hr;
			}

			VertShaderArr.push_back(m_vertexShader);

			++VSCounter;

			KFShadersBunch temp;

			BunchArr.push_back(temp); // с каждого нового вертекного шейдера добавляю новую связку шейдеров

			BunchArr[BunchArr.size() - 1].VS = m_vertexShader;
			BunchArr[BunchArr.size () - 1].VSBlobIndex = c; // Сохраняем индекс VS Shader для получения его Blob по запросу

		}
		if (ObjFormat[0] == 'p' && ObjFormat[1] == 's' && ObjFormat[2] == ' ') 
		{
			// Create the vertex shader from the buffer.
			hr = Local_D3DGC->DX_device->CreatePixelShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &m_pixelShader);
			if (FAILED( hr ))
			{
				RCUBE_ErrorMessageShow ( Prefix, PS_str, InitShader );
				return hr;
			}

			PixShaderArr.push_back(m_pixelShader);

			BunchArr[BunchArr.size() - 1].PS = m_pixelShader;
		}

		if (ObjFormat[0] == 'g' && ObjFormat[1] == 's' && ObjFormat[2] == ' ') 
		{
			// Create the vertex shader from the buffer.
			hr = Local_D3DGC->DX_device->CreateGeometryShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &m_geomShader);
			if (FAILED( hr ))
			{
				RCUBE_ErrorMessageShow ( Prefix, GS_str, InitShader );
				return hr;
			}

			GeomShaderArr.push_back(m_geomShader);


			BunchArr[BunchArr.size() - 1].GS = m_geomShader;
		}

		if (ObjFormat[0] == 'h' && ObjFormat[1] == 's' && ObjFormat[2] == ' ') 
		{
			// Create the vertex shader from the buffer.
			hr = Local_D3DGC->DX_device->CreateHullShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &m_hullShader);
			if (FAILED( hr ))
			{
				RCUBE_ErrorMessageShow ( Prefix, HS_str, InitShader );
				return hr;
			}

			HullShderArr.push_back(m_hullShader);


			BunchArr[BunchArr.size() - 1].HS = m_hullShader;
		}

		if (ObjFormat[0] == 'd' && ObjFormat[1] == 's' && ObjFormat[2] == ' ')
		{
			// Create the vertex shader from the buffer.
			hr = Local_D3DGC->DX_device->CreateDomainShader ( Blob->GetBufferPointer (), Blob->GetBufferSize (), NULL, &m_domainShader );
			if (FAILED ( hr ))
			{
				RCUBE_ErrorMessageShow ( Prefix, DS_str, InitShader );
				return hr;
			}

			DomainShderArr.push_back ( m_domainShader );


			BunchArr[BunchArr.size () - 1].DS = m_domainShader;
		}

		if (ObjFormat[0] == 'c' && ObjFormat[1] == 's' && ObjFormat[2] == ' ') 
		{
			// Create the vertex shader from the buffer.
			hr = Local_D3DGC->DX_device->CreateComputeShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &m_ComputeShader );
			if (FAILED( hr ))
			{
				RCUBE_ErrorMessageShow ( Prefix, CS_str, InitShader );
				return hr;
			}

			ComeputeShaderArr.push_back( m_ComputeShader );

			++CSCounter;
		}

		++c;
	}

// + Имена шейдеров
	int NamesAmount = VSCounter + CSCounter; // количество разных групп шейдеров
//	wstring * names = new  wstring[NamesAmount];

	WCHAR* TempName = new WCHAR[128];

	UCHAR* NamesStart = Buff + DescStart;

	size_t SizeOfWchar_t = sizeof(wchar_t);

	int Amount = 0;
	int SymbolCount = 0;

		VSCounter = 0;
		CSCounter = 0;

	while (Amount < ShadersNum)
	{
		SymbolCount = 0;
		while (*NamesStart != L'.')
		{
			memcpy((unsigned char*)&TempName[SymbolCount], NamesStart, SizeOfWchar_t);
			NamesStart += SizeOfWchar_t;
			++SymbolCount;
		}
		//  !!!!   ГЛЮКИ , если расширение файла больше 3 символов
		NamesStart += SizeOfWchar_t * 4; // расширение 3 символа пример : ".123"

		TempName[SymbolCount - 3] = NULL; // отсекаем _vs и _cs

		if (TempName[SymbolCount - 2] == 'v' || TempName[SymbolCount + 1] == 'V')
		{
			BunchArr[VSCounter].BunchName = StoreShaderName(TempName);
			++VSCounter;
		}
	
		if (TempName[SymbolCount - 2] == 'c' || TempName[SymbolCount + 1] == 'C')
		{
			ComeputeShaderNames.push_back(StoreShaderName(TempName));
			++CSCounter;
		}



		++Amount;
	}

	if ( CreateLayouts () )
	{
		ApplyLayoutsToShaderBunches ();
	}
	else
	{
		Result = E_FAIL;
	}

	delete[] TempName;
//	delete[] names;
// - Има шейдеров

	delete[] Buff;
	
	return Result;

}


void ResourceManager::ApplyLayoutsToShaderBunches ()
{
	size_t j = BunchArr.size ();
	for (size_t i = 0; i < j; ++i)
	{
			BunchArr[i].Layout = ShaderLayoutBind[i];
	}
}


bool ResourceManager::CreateLayouts ()
{
	ID3D11InputLayout* TempLayout;

	UINT numElements = _ARRAYSIZE ( Model3D_Layout );

	ID3D10Blob* TempBlob = GetShaderBlobByName ( L"ClusteredSM" );

	hr = Local_D3DGC->DX_device->CreateInputLayout ( Model3D_Layout, numElements, TempBlob->GetBufferPointer (),
		TempBlob->GetBufferSize (), &TempLayout );
	if (FAILED ( hr ))
	{
		RCUBE_ErrorMessageShow ( Prefix, L"Model3D_Layout", CreateLayout );
		return 0;
	}

	// Сохраняем Layout
	Layouts.push_back ( TempLayout );


	TempBlob = GetShaderBlobByName ( L"Font" );
	numElements = _ARRAYSIZE ( FlatObject_Layout );

	hr = Local_D3DGC->DX_device->CreateInputLayout ( FlatObject_Layout, numElements, TempBlob->GetBufferPointer (),
		TempBlob->GetBufferSize (), &TempLayout );
	if (FAILED ( hr ))
	{
		RCUBE_ErrorMessageShow ( Prefix, L"FlatObject_Layout", CreateLayout );
		return 0;
	}

	// Сохраняем Layout
	Layouts.push_back ( TempLayout );



	numElements = _ARRAYSIZE ( BB_Particle_Instance_Layout );

	TempBlob = GetShaderBlobByName ( L"ParticleAnimated" );

	hr = Local_D3DGC->DX_device->CreateInputLayout ( BB_Particle_Instance_Layout, numElements, TempBlob->GetBufferPointer (),
		TempBlob->GetBufferSize (), &TempLayout );
	if (FAILED ( hr ))
	{
		RCUBE_ErrorMessageShow ( Prefix, L"BB_Particle_Instance_Layout", CreateLayout );
		return 0;
	}

	// Сохраняем Layout
	Layouts.push_back ( TempLayout );

/*
	numElements = ARRAYSIZE ( TorchFire_Layout );

	TempBlob = GetShaderBlobByName ( L"TorchFire3D" );

	hr = Local_D3DGC->DX_device->CreateInputLayout ( TorchFire_Layout, numElements, TempBlob->GetBufferPointer (),
		TempBlob->GetBufferSize (), &TempLayout );
	if (FAILED ( hr ))
	{
		MessageBox ( Local_D3DGC->hwnd, L"TorchFire_Layout ошибка в создании лайаута.", L"Error", MB_OK );
		return 0;
	}

	// Сохраняем Layout
	Layouts.push_back ( TempLayout );
*/
	return 1;

}

wchar_t * ResourceManager::StoreShaderName(wchar_t* Name)
{
		int size = lstrlenW(Name);

		wchar_t *NamePointer = new wchar_t[size + 1 ];

		lstrcpyW( NamePointer, Name );

		return NamePointer;
}


int ResourceManager::GetShaderIndexByName(wchar_t* Name)
{
	int Bunchsize = (int)BunchArr.size();
	int CSsize = (int)ComeputeShaderNames.size();

	for (int i = 0; i < Bunchsize; ++i)
	{
		if ( !wcscmp(Name, BunchArr[i].BunchName) )
			return i;
	}

	for (int i = 0; i < CSsize; ++i)
	{
		if ( !wcscmp(Name, ComeputeShaderNames[i]) )
			return i;
	}

	return -1;
}


ID3D11InputLayout* ResourceManager::GetLayoutByIndex ( int LayoutNumber )
{
	if (LayoutNumber > (int)Layouts.size ())
		return NULL;

	return Layouts[LayoutNumber];
}


ID3D10Blob* ResourceManager::GetShaderBlobByName ( wchar_t* Name )
{
	int Bunchsize = (int)BunchArr.size ();
	for (int i = 0; i < Bunchsize; ++i)
	{
		if (!wcscmp ( Name, BunchArr[i].BunchName ))
			return BlobsArr[BunchArr[i].VSBlobIndex]; // Возвращаем индекс VS шейдера
	}
	return nullptr;
}


void ResourceManager::SetActiveShadersInProgramm(int ShadersIndex) 
{
	// If the same shader skeep setting it
	if ( ActiveShaderNumber != ShadersIndex )
	{
		// Setting Last shader for checking later in render. So we will not set same shader multiple times
		ActiveShaderNumber = ShadersIndex;

		KFShadersBunch *BunchArray = &BunchArr[ShadersIndex];
		if ( BunchArray->PS != NULL )
			Local_D3DGC->DX_deviceContext->PSSetShader ( BunchArray->PS, NULL, 0 );
		else
			Local_D3DGC->DX_deviceContext->PSSetShader ( NULL, NULL, NULL );

		if ( BunchArray->VS != NULL )
			Local_D3DGC->DX_deviceContext->VSSetShader ( BunchArray->VS, NULL, 0 );
		else
			Local_D3DGC->DX_deviceContext->VSSetShader ( NULL, NULL, NULL );

		if ( BunchArray->GS != NULL )
			Local_D3DGC->DX_deviceContext->GSSetShader ( BunchArray->GS, NULL, 0 );
		else
			Local_D3DGC->DX_deviceContext->GSSetShader ( NULL, NULL, NULL );

		if ( BunchArray->DS != NULL )
			Local_D3DGC->DX_deviceContext->DSSetShader ( BunchArray->DS, NULL, 0 );
		else
			Local_D3DGC->DX_deviceContext->DSSetShader ( NULL, NULL, NULL );

		if ( BunchArray->HS != NULL )
			Local_D3DGC->DX_deviceContext->HSSetShader ( BunchArray->HS, NULL, 0 );
		else
			Local_D3DGC->DX_deviceContext->HSSetShader ( NULL, NULL, NULL );

		if ( BunchArray->Layout > -1 )
			Local_D3DGC->DX_deviceContext->IASetInputLayout ( Layouts[( BunchArray->Layout )] );
		else
			Local_D3DGC->DX_deviceContext->IASetInputLayout ( NULL );
	}
}

int ResourceManager::InitOneShader( WCHAR * CSOFileName) {
	std::basic_ifstream<unsigned char> Readfile(CSOFileName, std::ios::in | std::ios::binary);
	if (!Readfile) {

		RCUBE_ErrorMessageShow ( Prefix, L"find shader .CSO file", InitOneShader_str );
		return -1;
	}

	int IndexShader = 0;

	Readfile.seekg(0, std::ios::end);
	int readNowFileSize = (int)Readfile.tellg();
	Readfile.seekg(0, std::ios::beg);

	ID3D10Blob* Blob;
	
	hr = D3DCreateBlob(readNowFileSize, &Blob);
	if ( FAILED( hr ) ) {
		RCUBE_ErrorMessageShow ( Prefix, L"Blob", InitOneShader_str );
		return -1;
	}

	Readfile.read(reinterpret_cast<unsigned char*>(Blob->GetBufferPointer()), readNowFileSize);

	BlobsArr.push_back(Blob);

	int Len = (int)wcslen((const wchar_t*)CSOFileName);

	if (CSOFileName[Len - 5] == 's' && CSOFileName[Len - 6] == 'v') {

		ID3D11VertexShader * m_vertexShader;

		KFShadersBunch temp;

		BunchArr.push_back(temp); // с каждого нового пиксельного шейдера добавляю новую связку шейдеров

		hr = Local_D3DGC->DX_device->CreateVertexShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &m_vertexShader);
		if ( FAILED( hr ) )
		{
			RCUBE_ErrorMessageShow ( Prefix, VS_str, InitOneShader_str );
			return -1;
		}

		VertShaderArr.push_back(m_vertexShader);
		BunchArr[BunchArr.size() - 1].VS = m_vertexShader;  // заполняю вертексный слот связки 

		IndexShader = (int)VertShaderArr.size();

	}

	if (CSOFileName[Len - 5] == 's' && CSOFileName[Len - 6] == 'p') 
	{

		ID3D11PixelShader * m_pixelShader;

		hr = Local_D3DGC->DX_device->CreatePixelShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &m_pixelShader);
		if ( FAILED( hr ) )
		{
			RCUBE_ErrorMessageShow ( Prefix, PS_str, InitOneShader_str );
			return -1;
		}

		PixShaderArr.push_back(m_pixelShader);
		BunchArr[BunchArr.size() - 1].PS = m_pixelShader;

		IndexShader = (int)PixShaderArr.size();

	}

	if (CSOFileName[Len - 5] == 's' && CSOFileName[Len - 6] == 'g') 
	{

		ID3D11GeometryShader * m_geomShader;

		hr = Local_D3DGC->DX_device->CreateGeometryShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &m_geomShader);
		if ( FAILED( hr ) )
		{
			RCUBE_ErrorMessageShow ( Prefix, GS_str, InitOneShader_str );
			return -1;
		}

		GeomShaderArr.push_back(m_geomShader);
		BunchArr[BunchArr.size() - 1].GS = m_geomShader;

		IndexShader = (int)GeomShaderArr.size();

	}

	if (CSOFileName[Len - 5] == 's' && CSOFileName[Len - 6] == 'h') 
	{

		ID3D11HullShader * m_hullShader;

		hr = Local_D3DGC->DX_device->CreateHullShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &m_hullShader);
		if ( FAILED( hr ) )
		{
			RCUBE_ErrorMessageShow ( Prefix, HS_str, InitOneShader_str );
			return -1;
		}

		HullShderArr.push_back(m_hullShader);
		BunchArr[BunchArr.size() - 1].HS = m_hullShader;

		IndexShader = (int)HullShderArr.size();

	}

	if (CSOFileName[Len - 5] == 's' && CSOFileName[Len - 6] == 'd')
	{

		ID3D11DomainShader * m_domainShader;

		hr = Local_D3DGC->DX_device->CreateDomainShader ( Blob->GetBufferPointer (), Blob->GetBufferSize (), NULL, &m_domainShader );
		if (FAILED ( hr ))
		{
			RCUBE_ErrorMessageShow ( Prefix, DS_str, InitOneShader_str );
			return -1;
		}

		DomainShderArr.push_back ( m_domainShader );
		BunchArr[BunchArr.size () - 1].DS = m_domainShader;

		IndexShader = (int)DomainShderArr.size ();

	}

	// этот шейдер в связки не добавляется!
	if (CSOFileName[Len - 5] == 's' && CSOFileName[Len - 6] == 'c') 
	{
		ID3D11ComputeShader * m_ComShader;

		hr = Local_D3DGC->DX_device->CreateComputeShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &m_ComShader);
		if ( FAILED( hr ) )
		{
			RCUBE_ErrorMessageShow ( Prefix, CS_str, InitOneShader_str );
			return -1;
		}

		ComeputeShaderArr.push_back(m_ComShader);

		IndexShader = (int)ComeputeShaderArr.size();


	}

	Readfile.close();

	return IndexShader - 1;
}

int ResourceManager::InitOneTexture( WCHAR * TextureFilename) {

	int Result = 0, length;

	Texture* NewTexture = new Texture;

	length = (UINT)wcslen(TextureFilename);

	if ((TextureFilename[length - 3] == 'p' && TextureFilename[length - 2] == 'n' && TextureFilename[length - 1] == 'g') ||
		(TextureFilename[length - 3] == 'j' && TextureFilename[length - 2] == 'p' && TextureFilename[length - 1] == 'g') ||
		(TextureFilename[length - 3] == 'b' && TextureFilename[length - 2] == 'm' && TextureFilename[length - 1] == 'p')) 
	{

		hr = DirectX::CreateWICTextureFromFile(Local_D3DGC->DX_device, TextureFilename, &NewTexture->Resource, &NewTexture->SRV, NULL);
		if ( FAILED( hr ) ) {
			RCUBE_ErrorMessageShow ( nullptr, L"WIC Texture file currupted.", nullptr );
			goto ERROR_END;
		}

	}
    if (TextureFilename[length - 3] == 'd' && TextureFilename[length - 2] == 'd' && TextureFilename[length - 1] == 's') 
	{

		hr = DirectX::CreateDDSTextureFromFile(Local_D3DGC->DX_device, TextureFilename, &NewTexture->Resource, &NewTexture->SRV, NULL);
		if ( FAILED( hr ) ) {
			RCUBE_ErrorMessageShow ( nullptr, L"DDS Texture file currupted.", nullptr );
			goto ERROR_END;
		}

	}

	Result = GetNewTextureIndex ( NewTexture );

	return Result;

ERROR_END:
	RCUBE_DELETE ( NewTexture );
	return -1;
}

void ResourceManager::decodeFile(unsigned char ** DestBuff , WCHAR * kafFilename , HWND hwnd) {

	std::basic_ifstream<unsigned char> Readfile(kafFilename, std::ios::in | std::ios::binary);


	if (!Readfile) 
	{
		RCUBE_ErrorMessageShow ( nullptr, L"No File found.", nullptr );
	}

	Readfile.seekg(0, std::ios::end);
	int size = (int)Readfile.tellg();
	Readfile.seekg(0, std::ios::beg);
	*DestBuff = new unsigned char[size];
	unsigned char * tempBuff = *DestBuff;
	unsigned char Temp;

	int c = 0;
	while (c < size) {

		Readfile.read(&Temp, 1);
		CharKey(&Temp);
		tempBuff[c] = Temp;
		
		++c;
	}

	Readfile.close();
}

void ResourceManager::CharKey(unsigned char * Check) {

	char Char = *Check;

	if (Char == 'K')
		*Check = 'A';

	if (Char == 'A')
		*Check = 'K';

}

HRESULT ResourceManager::InitSounds( WCHAR * kafFilename, SoundClass * ActiveSound) {

	unsigned char * Buff = nullptr, *temp, *DestPointer;
	decodeFile(&Buff, kafFilename, 0);
	unsigned char IsKaf[4];
	int DescStart, objNum;
	unsigned char ObjFormat[3];
	int ReadingFileSize = 0;

	temp = Buff;
	memcpy(IsKaf, temp, 4);
	temp += 4;

	if (IsKaf[0] != ' ' && IsKaf[1] != 'K' && IsKaf[2] != 'A' && IsKaf[3] != 'F') 
	{
		RCUBE_ErrorMessageShow ( nullptr, L"Texture .kaf file currupted.", InitSound );
		return E_FAIL;
	}


	memcpy((unsigned char*)&DescStart, temp, sizeof(int)); ///  читаю где начинается описание
	temp += sizeof(int);

	DestPointer = Buff + DescStart; // получаю указатель на начало массива который означает имена нужных мне файлов

	memcpy((unsigned char*)&objNum, temp, sizeof(int)); ///  читаю количество обьектов
	temp += sizeof(int);

	int c = 0;
	std::wstring * Names; //  массив имен обьектов

	// читаю имена обьктов************************************************************
	Names = new std::wstring[objNum];
	wchar_t TempwCH = L' ';

	while (c < objNum) {

		while (TempwCH != '.') {

			memcpy((unsigned char*)&TempwCH, DestPointer, sizeof(wchar_t));
			Names[c].push_back(TempwCH);
			DestPointer += sizeof(wchar_t);

		}

		memcpy((unsigned char*)&TempwCH, DestPointer, sizeof(wchar_t));
		Names[c].push_back(TempwCH);//           w   a
		DestPointer += sizeof(wchar_t);

		memcpy((unsigned char*)&TempwCH, DestPointer, sizeof(wchar_t));
		Names[c].push_back(TempwCH);//           a   g
		DestPointer += sizeof(wchar_t);

		memcpy((unsigned char*)&TempwCH, DestPointer, sizeof(wchar_t));
		Names[c].push_back(TempwCH);//           v   g
		DestPointer += sizeof(wchar_t);


		++c;
	}
	// читаю имена обьктов************************************************************

	c = 0;
	int length;

	IDirectSound3DBuffer * temp3DBuffer = nullptr;
	IDirectSoundBuffer8 * tempSoundbuff = nullptr;
	unsigned char * TempFile;
	// цикл непосредственно заполнения 3d и фонового массивов звуков*********************************************
	while (c < objNum) {

		memcpy((unsigned char*)&ObjFormat, temp, sizeof(unsigned char) * 3); ///  читаюформат файла
		temp += sizeof(unsigned char) * 3;

		memcpy((unsigned char*)&ReadingFileSize, temp, sizeof(int)); ///  читаю длинну файла
		temp += sizeof(int);

		TempFile = new unsigned char[ReadingFileSize ];


		memcpy(&TempFile[0], temp, ReadingFileSize); ///  читаю файл
		temp += ReadingFileSize;


		length = (int)Names[c].length();
		// если файл wav
		if (Names[c][length - 3] == 'w' && Names[c][length - 2] == 'a' && Names[c][length - 1] == 'v') {

			// если звук должен быть 3D
			if (Names[c][length - 7] == '_' && Names[c][length - 6] == '3' && Names[c][length - 5] == 'd') {

				ActiveSound->LoadMonoWaveFile(TempFile , &tempSoundbuff , &temp3DBuffer);

				ActiveSound->_3DSound.push_back(tempSoundbuff);
				ActiveSound->_3DSoundControl.push_back(temp3DBuffer);
			}

			// если звук должен быть фоновым
			else{

				ActiveSound->LoadStereoWaveFile(TempFile, &tempSoundbuff);
				ActiveSound->SoundStereo.push_back(tempSoundbuff);

			}

		}

		// если файл ogg
		if (Names[c][length - 3] == 'o' && Names[c][length - 2] == 'g' && Names[c][length - 1] == 'g') {

			// если звук должен быть 3D
			if (Names[c][length - 7] == '_' && Names[c][length - 6] == '3' && Names[c][length - 5] == 'd') {

				// тут пока не работает ничего

			}

			// если звук должен быть фоновым
			else {

				ActiveSound->LoadOggMusicFile(TempFile, &tempSoundbuff, ReadingFileSize);

				ActiveSound->SoundStereo.push_back(tempSoundbuff);

			}


		}

		delete[] TempFile;

		++c;
	}
	// цикл непосредственно заполнения 3d и фонового массивов звуков*********************************************

	delete[] Names;
	delete[] Buff;

	return S_OK;
}

void  ResourceManager::Init(D3DGlobalContext * g_D3DGC, TimerClass* _Timer, PhysXControler* PhysX )
{

	Local_D3DGC	= g_D3DGC;
	Local_Timer = _Timer;
	Local_PhysX = PhysX;
//	active_hwnd = Local_D3DGC->hwnd;
}

void ResourceManager::SetActiveComeputeShader(int ShaderIndex) 
{

	Local_D3DGC->DX_deviceContext->CSSetShader(ComeputeShaderArr[ShaderIndex] , NULL, 0);

}

void ResourceManager::SetNullAllShaders() {

	Local_D3DGC->DX_deviceContext->PSSetShader(NULL, NULL, 0);

	Local_D3DGC->DX_deviceContext->VSSetShader(NULL, NULL, 0);

	Local_D3DGC->DX_deviceContext->GSSetShader(NULL, NULL, 0);

	Local_D3DGC->DX_deviceContext->HSSetShader(NULL, NULL, 0);
    
	Local_D3DGC->DX_deviceContext->CSSetShader(NULL, NULL, 0);
}

ID3D11ComputeShader* ResourceManager::GetComputeShader(int ShaderIndex)
{
	if (ShaderIndex > (int)ComeputeShaderArr.size())
		return NULL;
	
	return ComeputeShaderArr[ShaderIndex];

}


ID3D11VertexShader*  ResourceManager::GetVertexShader ( int ShaderIndex )
{
	if (ShaderIndex > (int)VertShaderArr.size ())
		return NULL;

	return VertShaderArr[ShaderIndex];

}


// Создание буферов для объектов на сцене
int ResourceManager::Create_Flat_Obj_Buffers (int CPUAccess, UINT InstanceAmount, UINT IndexAmount = 0, DXTextureSRV* Texture = 0 )
{
	VertexBuffer <Vertex_FlatObject>* FlatObjectVB = nullptr;

	IndexBuffer < Index_Type >* TempIB = nullptr;

	Flat_Obj_Buffers *FlatObjBuffers = nullptr;

	// https://habrahabr.ru/company/xakep/blog/257891/
	// http://www.quizful.net/post/Inheritance-in-C++
	// https://habrahabr.ru/post/106294/  приведение типов
	// http://www.cplusplus.com/reference/vector/vector/ VECTORS
	int ReturnIndex;

	if (!UnusedFlatObjBuffersIndex.empty ())
	{
		FlatObjBuffers = new Flat_Obj_Buffers ();
		ReturnIndex = UnusedFlatObjBuffersIndex.back();
		UnusedFlatObjBuffersIndex.pop_back ();
		FlatObjectBuffers[ReturnIndex] = FlatObjBuffers;
	}
	else
	{
		ReturnIndex = (int)FlatObjectBuffers.size ();
		FlatObjBuffers = new Flat_Obj_Buffers ();
		FlatObjectBuffers.push_back ( FlatObjBuffers );
	}

		FlatObjectVB = new VertexBuffer <Vertex_FlatObject> ( Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, CPUAccess, InstanceAmount, nullptr );			// Создаём буфер вертексов FlatObject
		FlatObjBuffers->FlatObjectVB = FlatObjectVB;


	if (IndexAmount > 0)
	{
		TempIB = new IndexBuffer < Index_Type > ( Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, CPUAccess, IndexAmount, FlatObjectIndices);
		FlatObjBuffers->IndexBs = TempIB;
	}

	FlatObjBuffers->RenderTexture = Texture;

	FlatObjBuffers->ThisBufferIndex = ReturnIndex; // Сохраняем собственный индекс в своём теле

	return ReturnIndex;

}


bool ResourceManager::Delete_Flat_ObjectBuffers ( int Index )
{
	int i = (int)FlatObjectBuffers.size ();
	if (Index < i)
	{
		UnusedFlatObjBuffersIndex.push_back ( Index );
		RCUBE_DELETE ( FlatObjectBuffers[Index] );
		return true;
	}

	return false;
}


Flat_Obj_Buffers* ResourceManager::Get_Flat_ObjectBuffers_ByIndex ( int Index )
{
	int i = (int)FlatObjectBuffers.size ();

	if (Index < i)
		return FlatObjectBuffers[Index];
	else
		return nullptr;
}


bool ResourceManager::DeleteTexture ( int Index )
{
	int i = (int)TexturesArr.size ();
	if (Index < i)
	{
		UnusedTexturesIndex.push_back ( Index );
		RCUBE_DELETE ( TexturesArr[Index] );
		return true;
	}
	else
		return false;
}


bool ResourceManager::AddMenu ( Menu* NewMenu )
{
	Menus.push_back ( NewMenu );
	return true;
}


int ResourceManager::CreateTexture ( TextureData& _Data )
{
	int ReturnIndex;

	Texture* NewTexture = new Texture;

	D3D11_TEXTURE2D_DESC sharedTexDesc;
	ZeroMemory ( &sharedTexDesc, sizeof ( sharedTexDesc ) );

	sharedTexDesc.Width = _Data.Width;
	sharedTexDesc.Height = _Data.Height;
	sharedTexDesc.Format = (DXGI_FORMAT)_Data.Format;// DXGI_FORMAT_R8G8B8A8_UNORM;
	sharedTexDesc.MipLevels = _Data.MipMapLevels;
	sharedTexDesc.ArraySize = _Data.ArraySize;
	sharedTexDesc.SampleDesc.Count = _Data.SampleDesc.Count;
	sharedTexDesc.SampleDesc.Quality = _Data.SampleDesc.Quality;
	sharedTexDesc.Usage = (D3D11_USAGE)_Data.Usage;// D3D11_USAGE_DEFAULT; //D3D11_USAGE_IMMUTABLE;

	switch (sharedTexDesc.Usage)
	{
	case D3D11_USAGE_DYNAMIC:
		sharedTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		break;
	case D3D11_USAGE_STAGING:
		sharedTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		break;
	default:
		sharedTexDesc.CPUAccessFlags = 0;
	}

	_Data.ShaderResource ? sharedTexDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE : sharedTexDesc.BindFlags;
	_Data.DepthStensil ? sharedTexDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL : sharedTexDesc.BindFlags;
	_Data.RenderTarget ? sharedTexDesc.BindFlags |= D3D11_BIND_RENDER_TARGET : sharedTexDesc.BindFlags;
	_Data.Unordered_Access ? sharedTexDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS : sharedTexDesc.BindFlags;

	_Data.Shared_KeyMutex ? sharedTexDesc.MiscFlags |= D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX : sharedTexDesc.MiscFlags;
	_Data.TextureCube ? sharedTexDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE : sharedTexDesc.MiscFlags;
	_Data.GenerateMipMaps ? sharedTexDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS : sharedTexDesc.MiscFlags;


	if (_Data.Type == _2D)
	{
		hr = Local_D3DGC->DX_device->CreateTexture2D ( &sharedTexDesc, NULL, &NewTexture->Texture2D );
		if (FAILED ( hr ))
		{
			RCUBE_ErrorMessageShow ( Prefix, L"2D Texture", CreateTexture_str );
			goto ERROR_END;
		}

		NewTexture->Resource = dynamic_cast<ID3D11Resource*>(NewTexture->Texture2D); // Resource created

	}

	if (_Data.ShaderResource)
	{
		hr = Local_D3DGC->DX_device->CreateShaderResourceView ( NewTexture->Resource, NULL, &NewTexture->SRV );
		if (FAILED ( hr ))
		{
			RCUBE_ErrorMessageShow ( Prefix, L"ShaderResourceView", CreateTexture_str );
			goto ERROR_END;
		}

	}

	if (_Data.RenderTarget)
	{
		hr = Local_D3DGC->DX_device->CreateRenderTargetView ( NewTexture->Resource, NULL, &NewTexture->RTV );
		if (FAILED ( hr ))
		{
			RCUBE_ErrorMessageShow ( Prefix, L"RenderTargetView", CreateTexture_str );
			goto ERROR_END;
		}

	}

	if (_Data.DepthStensil)
	{
		hr = Local_D3DGC->DX_device->CreateDepthStencilView ( NewTexture->Resource, NULL, &NewTexture->DSV );
		if (FAILED ( hr ))
		{
			RCUBE_ErrorMessageShow ( Prefix, L" DepthStencilView", CreateTexture_str );
			goto ERROR_END;
		}

	}

	if (_Data.Unordered_Access )
	{
		hr = Local_D3DGC->DX_device->CreateUnorderedAccessView ( NewTexture->Resource, NULL, &NewTexture->UAV );
		if (FAILED ( hr ))
		{
			RCUBE_ErrorMessageShow ( Prefix, L" UnorderedAccessView", CreateTexture_str );
			goto ERROR_END;
		}

	}

// Сохраняем данные созданной текстуры в её описании
	NewTexture->DepthStensil	= _Data.DepthStensil;
	NewTexture->ShaderResource  = _Data.ShaderResource;
	NewTexture->Format			= _Data.Format;
	NewTexture->Height			= _Data.Height;
	NewTexture->Width			= _Data.Width;
	NewTexture->RenderTarget	= _Data.RenderTarget;
	NewTexture->Shared_KeyMutex = _Data.Shared_KeyMutex;
	NewTexture->TextureCube		= _Data.TextureCube;
	NewTexture->Type			= _Data.Type;
	NewTexture->Unordered_Access= _Data.Unordered_Access;
	NewTexture->Usage			= _Data.Usage;
//	NewTexture->ArraySize		= _Data.ArraySize;
//	NewTexture->MipMapLevels	= _Data.MipMapLevels;

// Сохраняем созданные ресурсы текстуры в ощем списке текстур и вызвращяем её индекс
	ReturnIndex = GetNewTextureIndex ( NewTexture );

return ReturnIndex;

ERROR_END:
	RCUBE_DELETE  ( NewTexture );
	return -1;
}


int ResourceManager::CloneTextureStaging ( int Source )
{
	int Index = -1;

	size_t i = TexturesArr.size ();
	if (Source < i )
	{
		// Проверяем создана ли текстура как STAGING
		if (TexturesArr[Source]->Usage == D3D11_USAGE_STAGING)
			return -2;	// Текстура создана как STAGING

		if (TexturesArr[Source]->ShaderResource == false )
			return -1;
		
		TextureData NewTexture;
		NewTexture.Format = TexturesArr[Source]->Format;
		NewTexture.Height = TexturesArr[Source]->Height;
		NewTexture.Width = TexturesArr[Source]->Width;
		NewTexture.Depth = 0;
		NewTexture.ArraySize = 1;
		NewTexture.MipMapLevels = 1;
		NewTexture.Type = TexturesArr[Source]->Type;
		NewTexture.SampleDesc.Count = 1;
		NewTexture.SampleDesc.Quality = 0;
		NewTexture.Usage = D3D11_USAGE_STAGING;
		NewTexture.Shared_KeyMutex = false;
		NewTexture.DepthStensil = false;
		NewTexture.GenerateMipMaps = false;
		NewTexture.RenderTarget = false;
		NewTexture.ShaderResource = false;
		NewTexture.TextureCube = false;
		NewTexture.Unordered_Access = false;
	
		Index = CreateTexture ( NewTexture );

	}
	return Index;
}


// Сохранить текстуру в файл
bool ResourceManager::SaveTextureToFile ( int Index, WCHAR* Name )
{
	bool CloneDone = true;
	int TextureIndex = -1;
	size_t i = TexturesArr.size ();
	if (Index < i)

	if (TexturesArr[Index]->Texture2D == nullptr)
		return false;

	if (TexturesArr[Index]->Usage != D3D11_USAGE_STAGING)
	{
		TextureIndex = CloneTextureStaging ( Index );

		if (TextureIndex == -2)		// Если текстура создана как STAGING
		{
			TextureIndex = Index;
			CloneDone = false;
		}
		else if (TextureIndex == -1)// Если не получилось клонировать текстуру
			return false;
	}

	Local_D3DGC->DX_deviceContext->CopyResource( TexturesArr[TextureIndex]->Resource, TexturesArr[Index]->Resource );

	D3D11_MAPPED_SUBRESOURCE  mapResource;
	hr = Local_D3DGC->DX_deviceContext->Map ( TexturesArr[TextureIndex]->Resource, 0, D3D11_MAP_READ, NULL, &mapResource );
	if (hr == S_OK)
	{
		Image img;
		img.height = TexturesArr[TextureIndex]->Height;
		img.width = TexturesArr[TextureIndex]->Width;
		img.format = (DXGI_FORMAT)TexturesArr[TextureIndex]->Format;
		img.rowPitch = mapResource.RowPitch;
		img.slicePitch = mapResource.DepthPitch;
		img.pixels = (UCHAR*)mapResource.pData;

		hr = SaveToWICFile ( img, WIC_FLAGS_NONE, GetWICCodec ( WIC_CODEC_PNG ), Name );
		if (FAILED ( hr ))
		{
			assert ( L"Could not save Texture to file." );
		}
		Local_D3DGC->DX_deviceContext->Unmap ( TexturesArr[TextureIndex]->Resource, 0);
	}

	if (CloneDone)
		DeleteTexture ( TextureIndex );

	return true;
}

void  ResourceManager::BuildSentanceVertexArray ( BuildSentanceData* BuildData)
//void ResourceManager::BuildSentanceVertexArray ( FontClass* Font, Vertex_FlatObject* vertices, char* sentence, float& drawX, float& drawY, float& RenderFontSize, XMVECTOR& Colour )
{
	int numLetters, letter;

//	RCube_VecFloat234 colour;
	
//	colour.Vec = BuildData->Colour;

	// Get the number of letters in the sentence.
	numLetters = (int)strlen ( BuildData->text );

	// The following loop will now build the vertex and index arrays. It takes each character from the sentence and creates two triangles for it. It then maps the character from the font texture onto 
	// those two triangles using the m_Font array which has the TU texture coordinates and pixel size. Once the polygon for that character has been created it then updates the X coordinate on the 
	// screen of where to draw the next character.

	// Draw each letter onto a quad.
	for (int i = 0; i < numLetters; ++i)
	{
		//  +++++++++   Добавлена поддержка ASCII символов после 127   +++++++++++
		char *Source = &BuildData->text[i];

		if (*Source < 0)
			letter = ((int)*Source) + 159; // 191 - 32
		else
			letter = ((int)*Source) - 32;
		// Беква Ё
		if (*Source == -88)
			letter = 159;
		// Бука ё
		else if (*Source == -72)
			letter = 160;
		//  --------   Добавлена поддержка ASCII символов после 127   -----------

		// If the letter is a space then just move over three pixels.
		if (letter == 0)
		{
			// Ширина пробела
			BuildData->drawX += BuildData->Font->FontSize / 2;
		}
		else
		{
			_Symbol_Dim *Source = &BuildData->Font->Symbols[letter - 1];
			float DYRFS = BuildData->drawY - BuildData->RenderFontSize;
			float DXSS = BuildData->drawX + Source->Symbol_Width;
			// First triangle in quad.
			BuildData->vertexes->Position = XMFLOAT3 ( BuildData->drawX, BuildData->drawY, 0.0f );  // Top left.
			BuildData->vertexes->TexCoord = XMFLOAT4 ( Source->Start, 0.0f, 0.0f, BuildData->Colour.w );
			++BuildData->vertexes;

			BuildData->vertexes->Position = XMFLOAT3 ( ( BuildData->drawX + Source->Symbol_Width), DYRFS, 0.0f );  // Bottom right.
			BuildData->vertexes->TexCoord = XMFLOAT4 ( Source->End, 1.0f, 0.0f, BuildData->Colour.w );
			++BuildData->vertexes;

			BuildData->vertexes->Position = XMFLOAT3 ( BuildData->drawX, DYRFS, 0.0f );  // Bottom left.
			BuildData->vertexes->TexCoord = XMFLOAT4 ( Source->Start, 1.0f, 0.0f, BuildData->Colour.w );
			++BuildData->vertexes;

			// Second triangle in quad.
			BuildData->vertexes->Position = XMFLOAT3 ( BuildData->drawX, BuildData->drawY, 0.0f );  // Top left.
			BuildData->vertexes->TexCoord = XMFLOAT4 ( Source->Start, 0.0f, 0.0f, BuildData->Colour.w );
			++BuildData->vertexes;

			BuildData->vertexes->Position = XMFLOAT3 ( DXSS, BuildData->drawY, 0.0f );  // Top right.
			BuildData->vertexes->TexCoord = XMFLOAT4 ( Source->End, 0.0f, 0.0f, BuildData->Colour.w );
			++BuildData->vertexes;

			BuildData->vertexes->Position = XMFLOAT3 ( DXSS, DYRFS, 0.0f );  // Bottom right.
			BuildData->vertexes->TexCoord = XMFLOAT4 ( Source->End, 1.0f, 0.0f, BuildData->Colour.w );
			++BuildData->vertexes;

			// Update the x location for drawing by the size of the letter and one pixel.
			BuildData->drawX = BuildData->drawX + Source->Symbol_Width + 1.0f;
		}
	}

	return;
}


int ResourceManager::GetNewSentenceIndex ( SentenceType* NewSentence )	// Получить свободный индекс предложения в системе
{
	return GetObjectBuffer <vector<SentenceType*>, vector<UINT>, SentenceType* > ( RCube_Sentences, UnusedSentenceIndex, NewSentence );
}


bool ResourceManager::InitializeSentence ( SentenceType* sentence, int& maxLength )
{
	// Set the maximum length of the sentence.
	sentence->MaxLength = maxLength;

	// Set the number of vertices in the vertex array.
	sentence->vertexCount = 6 * maxLength;

	sentence->VertexBufferIndex = Create_Flat_Obj_Buffers ( NO_CPU_ACCESS_BUFFER, sentence->vertexCount, 0, TexturesArr[RCube_Font[sentence->FontType]->FontTextureIndex]->SRV );

	return true;
}


void ResourceManager::UpdateSentence (int SentenceNumber, char* text)
{
	UpdateSentence (SentenceNumber, text, RCube_Sentences[SentenceNumber]->PosX, RCube_Sentences[SentenceNumber]->PosY);
}


void ResourceManager::UpdateSentence ( int SentenceNumber, char* text, int positionX, int positionY )// , float RenderFontSize )
{
	// Checking sentence index in range
//	if ( !(SentenceNumber < (int)RCube_Sentences.size ()) )
//		return;

	int numLetters;
	Vertex_FlatObject* vertices;

	BuildSentanceData* BuildData = new BuildSentanceData;

	// Get the number of letters in the sentence.
	numLetters = (int)strlen ( text );

	SentenceType *Source = RCube_Sentences[SentenceNumber];

	// Check for possible buffer overflow.
	if (numLetters > Source->MaxLength)
	{
		text[Source->MaxLength - 1] = NULL;
	}
	// копируем новый текст предложения в объект предложения ( скролинг бе этого не работает )
	strcpy_s ( Source->Text, Source->MaxLength, text );

	// Create the vertex array.
	vertices = new Vertex_FlatObject[Source->vertexCount];

	// Initialize vertex array to zeros at first.
	memset ( vertices, 0, (sizeof ( Vertex_FlatObject ) * Source->vertexCount) );

	// Обновляем позицию предложения 
	Source->PosX = positionX;
	Source->PosY = positionY;

	// Calculate the starting location to draw the sentence on the screen at.
	// Calculate the X and Y pixel position on the screen to start drawing to.
//	drawX = (float)(((Local_D3DGC->ScreenWidth / 2) * -1) + positionX);
//	drawY = (float)((Local_D3DGC->ScreenHeight / 2) - positionY);

	BuildData->Colour = Source->Colour;
	BuildData->drawX = ( float ) ( ( ( Local_D3DGC->ScreenWidth / 2 ) * -1 ) + positionX );
	BuildData->drawY = ( float ) ( ( Local_D3DGC->ScreenHeight / 2 ) - positionY );
	BuildData->Font = RCube_Font[Source->FontType];
	BuildData->RenderFontSize = RCube_Font[Source->FontType]->FontSize;// RenderFontSize;
	BuildData->text = text;
	BuildData->vertexes = vertices;

	// Build the vertex array using the FontClass and the sentence information.
	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	BuildSentanceVertexArray ( BuildData );
//	BuildSentanceVertexArray ( RCube_Font[Source->FontType], vertices, text, drawX, drawY, RenderFontSize, Source->Colour );

	// Copy the vertex array information into the sentence vertex buffer.
	FlatObjectBuffers[Source->VertexBufferIndex]->FlatObjectVB->Update ( vertices );

	RCUBE_DELETE ( BuildData );
	// Release the vertex array as it is no longer needed.
	RCUBE_ARR_DELETE ( vertices );

}


int ResourceManager::AddSentence ( SENTENCE_INIT_DATA* data, char* String )
{
	bool result;
	// Если текст больше или равен максимальной длинне строки позволенной в классе STRMAXLENGTH или 
	// строка инициализации больше указанной желаемой длинны строки, или максимальная длинна строки
	// меньше 1 , то возвращаем -1 вместо номера строки в массиве - строка не создаётся
	int Size = (int)strlen ( String );
	if (data->MaxLength > STRMAXLENGTH || Size > STRMAXLENGTH || data->MaxLength < 0)
	{
		return -1;
	}
	else if (data->MaxLength == 0 || Size + 2 > data->MaxLength) // Size + 2 - для мигающего курсора + \0 
	{
		data->MaxLength = Size + 2;
	}

	SentenceType* Sentence1 = new SentenceType;
	ZeroMemory ( Sentence1, sizeof ( SentenceType ) );

	int Num = GetNewSentenceIndex ( Sentence1 );

	SentenceType* Source = RCube_Sentences[Num];

	result = InitializeSentence ( Source, data->MaxLength );
	if (!result)
		return -1;

	Source->Colour = data->Colour;
	Source->FontType = data->FontType;
	Source->Render = data->Render;
	Source->MaxLength = data->MaxLength;
	Source->PosX = ( int ) ( data->PosX * Local_D3DGC->ScreenScale.z );
	Source->PosY = ( int ) ( data->PosY * Local_D3DGC->ScreenScale.w );
	Source->ShowType = data->ShowType;
	Source->HideType = data->HideType;
	Source->Level = data->Level;
	Source->Height = ( UINT ) (RCube_Font[data->FontType]->FontHeightInPixel * Local_D3DGC->ScreenScale.w);

	if (data->ScrollSpeed == 0)	Source->ScrollSpeed = 30.0f;

	strcpy_s ( Source->Text, Source->MaxLength, String );

	UpdateSentence ( Num, Source->Text, Source->PosX, Source->PosY);

	return Num;
}


int ResourceManager::DeleteSentence ( int SentenceNumber )
{
	int i = (int)RCube_Sentences.size ();
	if (SentenceNumber < i)
	{
		Delete_Flat_ObjectBuffers ( RCube_Sentences[SentenceNumber]->VertexBufferIndex );
		UnusedSentenceIndex.push_back ( SentenceNumber );
		delete RCube_Sentences[SentenceNumber];
		return 0;
	}
	else
		return -1;

}


void ResourceManager::DeleteAllSentences ()
{
	int i = 0;
	int j = (int)RCube_Sentences.size ();

	if (j > 0)
		do
		{
			Delete_Flat_ObjectBuffers ( RCube_Sentences[i]->VertexBufferIndex );
			UnusedSentenceIndex.push_back ( i );
			delete RCube_Sentences[i];
			++i;
		} while (i < j);

		RCube_Sentences.clear ();
}


void ResourceManager::GetSentencePos (int SentenceNumber, RCube_XY_POS* XY_Position)
{
	XY_Position->X = RCube_Sentences[SentenceNumber]->PosX;
	XY_Position->Y = RCube_Sentences[SentenceNumber]->PosY;
}


int ResourceManager::GetSentencePosX ( int SentenceNumber )
{
	return RCube_Sentences[SentenceNumber]->PosX;
}


int ResourceManager::GetSentencePosY ( int SentenceNumber )
{
	return RCube_Sentences[SentenceNumber]->PosY;
}


char* ResourceManager::GetSentenceText ( int SentenceNumber )
{
	return RCube_Sentences[SentenceNumber]->Text;
}


int ResourceManager::GetSentenceMaxLength ( int SentenceNumber )
{
	int i = (int)RCube_Sentences.size ();
	if (SentenceNumber < i)
	{
		return (RCube_Sentences[SentenceNumber]->MaxLength);
	}
	else
		return -1;
}


UINT ResourceManager::GetSentenceHeight ( int SentenceNumber )
{
	return RCube_Sentences[SentenceNumber]->Height;
}


int ResourceManager::GetFontHeightInPixels ( int FontNumber )
{
	int i = (int)RCube_Font.size ();
	if (FontNumber < i)
	{
		return (RCube_Font[FontNumber]->FontHeightInPixel);
	}
	else
		return -1;
}



int ResourceManager::Create_3D_Obj_Mesh_Buffers( int CPUAccess, UINT InstanceAmount, UINT IndexAmount = 0 )
{
	_3D_Obj_Buffers *_3DObjBuffers = new _3D_Obj_Buffers ();
	
	int ReturnIndex = GetNew3D_Obj_Mesh_Buffer_Index( _3DObjBuffers );

	_3DObjBuffers->Vertexes = new VertexBuffer <Vertex_Model3D> ( Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, CPUAccess, InstanceAmount, nullptr);			// Создаём буфер вертексов FlatObject

	if (IndexAmount > 0)
	{
		_3DObjBuffers->Indexes = new IndexBuffer < Index_Type > ( Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, CPUAccess, IndexAmount, nullptr);
	}

	return ReturnIndex;

}


bool ResourceManager::LoadKFObject ( std::wstring FileName, _3DModel* New_3D_Model )
{

	RCube_VecFloat234 TempFloat3;
	RCube_VecFloat234 TempFloat3_2;

	std::basic_ifstream< UCHAR > file ( FileName, std::ios::in | std::ios::binary );
	if ( !file )
	{
		MessageBox ( NULL, L"Can't open .kfo for reading.", Error, MB_OK );
		return false;
	}

	// Читаем из файла количество мешей в модели
	UINT MeshesNumber = 0;
	file.read ( ( UCHAR* ) &MeshesNumber, sizeof ( int ) ); // _3DModels.MeshesCount
	// Если количество мешей в модели больше MAX_MODEL_MESH
	if ( MeshesNumber > MAX_MESH_IN_MODEL || MeshesNumber == 0 )
	{
		MessageBox ( NULL, L"Количество мешей в модели больше MAX_MODEL_MESH или равно нулю.", Error, MB_OK );
		return false;
	}

	int TextureSize = NULL; // размер тектуры которую я буду читать
	UCHAR* TextuteFile = nullptr; // это файл текстуры который я читаю из общей кучи
	int g = 0;

	// Загружаем все меши и текстуры
	for ( UINT i = 0; i < MeshesNumber; ++i )
	{
		MeshData* NewMesh = new MeshData;	// Создаём объект Mesh модели и сохраняем его в списке мешей объекта
		New_3D_Model->Meshes.push_back ( NewMesh );

		// Читаем количество вертексов в меше
		file.read ( ( UCHAR* ) &NewMesh->VertexBufferSize, sizeof ( int ) ); // читаю размер следующего вертексного буферы
		New_3D_Model->ObjectVertexesCout += NewMesh->VertexBufferSize; // нахожу сумму вершин всех ранее созданных мной обьектов
		// Читаем количество индексов в меше
// !!!!!!!!!!!!!!!!!  НУЖНО ДЕЛАТЬ  !!!
		//		file.read ( ( UCHAR* ) &NewMesh->IndexBufferSize, sizeof ( int ) ); // читаю размер следующего индексного буферы
		
		// Создаём VertexBuffer для загруженного меша и сохраняем его индекс
		NewMesh->BufferIndex = Create_3D_Obj_Mesh_Buffers ( CPU_ACCESS_BUFFER, NewMesh->VertexBufferSize );

		// Загружаем вертексы меша из KFO в буфер
		Vertex_Model3D* Buffer = _3DModelsMeshBuffers[NewMesh->BufferIndex]->Vertexes->MapDiscard ();
		file.read ( ( UCHAR* ) &Buffer[0], sizeof ( Vertex_Model3D ) * NewMesh->VertexBufferSize );//чтение одним махом
		_3DModelsMeshBuffers[NewMesh->BufferIndex]->Vertexes->Unmap ();
// Загружаем индексы меша  из KFO в буфер ( если они есть )
//
//
//
//

		file.read ( ( UCHAR* ) &TextureSize, sizeof ( int ) );

		// Если для меша есть текстура, то создаём и загружаем её
		if ( TextureSize )
		{
			ID3D11Resource * TextureResurce;
			ID3D11ShaderResourceView * TextureSRV;

			// чтение и создание текстуры
			TextuteFile = new UCHAR [ TextureSize ];
			file.read ( TextuteFile, TextureSize * sizeof ( UCHAR ) );

			hr = DirectX::CreateWICTextureFromMemory ( Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, &TextuteFile [ 0 ], ( size_t ) TextureSize, &TextureResurce
													   , &TextureSRV, NULL );
			if ( hr != S_OK ) // E_FAIL
			{
				MessageBox ( 0, L"3D Model Texture creation failure", Error, MB_OK );
			}
			else
			{
/*
#if defined( DEBUG ) || defined( _DEBUG )
				const char c_szName0 [] = "Model Texture Resource MAN";
				NewMesh->TextureResurce->SetPrivateData ( WKPDID_D3DDebugObjectName, sizeof ( c_szName0 ) - 1, c_szName0 );
				const char c_szName1 [] = "Model Texture SRV Resource MAN";
				NewMesh->TextureSRV->SetPrivateData ( WKPDID_D3DDebugObjectName, sizeof ( c_szName0 ) - 1, c_szName1 );

#endif
*/
				Texture* NewTexture = new Texture;
				NewTexture->Resource = TextureResurce;
				NewTexture->SRV = TextureSRV;

				NewMesh->TextureIndex = GetNewTextureIndex ( NewTexture );	// Сохраняем текстуру и её номер для меша
			}


			RCUBE_ARR_DELETE ( TextuteFile );
		}

		// чтние баундинг бокса (для конкретного мэша)

		file.read ( ( UCHAR* ) &TempFloat3, sizeof ( XMFLOAT3 ) ); // читаю минимальное
		file.read ( ( UCHAR* ) &TempFloat3_2, sizeof ( XMFLOAT3 ) ); // читаю максимальное

		NewMesh->MeshBox.CreateBoundStruct ( TempFloat3, TempFloat3_2 );

	}

	file.read ( ( UCHAR* ) &TempFloat3, sizeof ( XMFLOAT3 ) ); // читаю минимальное
	file.read ( ( UCHAR* ) &TempFloat3_2, sizeof ( XMFLOAT3 ) ); // читаю максимальное

	New_3D_Model->ObjectBox.CreateBoundStruct ( TempFloat3, TempFloat3_2 );

	file.close ();

	return true;
}


int ResourceManager::GetNew3D_Obj_Index ( _3DModel* NewModel )
{
	return NewModel->_3D_Obj_Index = GetObjectBuffer <vector < _3DModel* >, vector<UINT>, _3DModel* > ( _3DModels, Unused3DModelsIndex, NewModel );
}


int ResourceManager::GetNew3D_Obj_Mesh_Buffer_Index ( _3D_Obj_Buffers* NewBuffer )
{
	return NewBuffer->ThisBufferIndex = GetObjectBuffer <vector < _3D_Obj_Buffers* >, vector<UINT>, _3D_Obj_Buffers* > ( _3DModelsMeshBuffers, Unused3DModelsMeshBuffersIndex, NewBuffer );
}


int ResourceManager::Add_3D_Object ( std::wstring FileNameKFO, int InstCount = 1 ) 
{
	int Result = -1;

	if ( InstCount < 1 )
	{
		MessageBox ( 0, L"Количество объектов 3D Model не может быть нулевым. Создаю одну модель", Error, MB_OK );
		InstCount = 1;
//		return -1;
	}

	_3DModel* Model3DPointer;
	int c = 0;
	Model3DPointer = new _3DModel;

	Model3DPointer->InstanceCount = InstCount;

	// Загружаем модель
	if ( !LoadKFObject ( FileNameKFO, Model3DPointer ) )
	{
		RCUBE_DELETE ( Model3DPointer );
		return -1;
	}

	size_t Size = sizeof ( PositionType ) * InstCount ;

	Model3DPointer->Positions = new PositionType [ InstCount ];
	Model3DPointer->PositionsDraw = new  PositionType [ InstCount ];

	memset ( Model3DPointer->Positions, 0, Size );
	memset ( Model3DPointer->PositionsDraw, 0, Size );
	
	// Создаём буфер позиций и поворотов для Instance 3D модели
	Model3DPointer->InstancePositionsBuffer = new VertexBuffer <PositionType> ( Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, CPU_ACCESS_BUFFER, InstCount, nullptr);
	
	Result = GetNew3D_Obj_Index ( Model3DPointer );

	return Result;
}


int ResourceManager::Delete_3D_Object_Meshes_Buffers ( size_t ObjectIndex )
{
	size_t i = _3DModels.size ();

	if ( ObjectIndex < i )
	{
		size_t j = _3DModels [ObjectIndex]->Meshes.size ();
		int c = 0;
		while ( c < j )
		{
			int BufferIndextoDelete = _3DModels[ObjectIndex]->Meshes[c]->BufferIndex;
			// Сохраняем освободившийся индекс буферы меша
			Unused3DModelsMeshBuffersIndex.push_back ( BufferIndextoDelete );
			// Удаляем текстуру от меша и сохраняем её номер в свободных
			DeleteTexture ( _3DModels[ObjectIndex]->Meshes[c]->TextureIndex );

// С МАТЕРИАЛАМИ ПРИДЁТСЯ ЕЩЁ ЧТО_ТО МУТИТЬ ПО УДАЛЕНИЮ !!!!
			// Удаляем сам меш
			RCUBE_DELETE ( _3DModels[ObjectIndex]->Meshes[c] );
			// Удаляем буфер для меша
			RCUBE_DELETE ( _3DModelsMeshBuffers[BufferIndextoDelete] );
			++c;
		}
		return 0;
	}
	return -1;
}


int ResourceManager::Delete_3D_Object ( size_t ObjectIndex )
{
	size_t i = _3DModels.size ();

	if ( ObjectIndex < i )
	{
		// Сохраняем освободившийся индекс 3D модели
		Unused3DModelsIndex.push_back ( _3DModels[ObjectIndex]->_3D_Obj_Index );
		// Удаляем все : буферы , текстуры , материалы модели
		Delete_3D_Object_Meshes_Buffers ( ObjectIndex );
		// Удаляем модель
		RCUBE_DELETE ( _3DModels[ObjectIndex] );
		return 0;
	}
	return -1;
}


_3D_Obj_Buffers* ResourceManager::Get_Mesh_Buffers_By_Index ( int ObjectIndex )
{
	size_t i = _3DModelsMeshBuffers.size ();

	if ( ObjectIndex < i )
	{
		return _3DModelsMeshBuffers[ObjectIndex];
	}
	return nullptr;
}


_3DModel* ResourceManager::Get3D_Object_Pointer ( int ObjectIndex )
{
	size_t i = _3DModels.size ();

	if ( ObjectIndex < i )
	{
		return _3DModels[ObjectIndex];
	}
	return nullptr;
}



bool ResourceManager::Update_3D_Object_InstancesPosBuffer ( int ObjectIndex, int CountingFromInstanceIndex, int Amount  )
{

	_3DModel* _3D_Model = _3DModels[ObjectIndex];

	PositionType* IndexPtr = _3D_Model->InstancePositionsBuffer->MapDiscard ();

	size_t ElementSize = sizeof ( PositionType );

	if ( CountingFromInstanceIndex < 0 )
		memcpy ( IndexPtr, _3D_Model->PositionsDraw, ElementSize * Amount );
	
	else 
		memcpy ( IndexPtr + CountingFromInstanceIndex * ElementSize, _3D_Model->PositionsDraw, ElementSize );

	_3D_Model->InstancePositionsBuffer->Unmap ();

	return true;
}


void ResourceManager::Set_3D_Object_Positon ( int IndexOfObject, int InstanceIndex, float PosX, float PosY, float PosZ )
{

	_3DModels[IndexOfObject]->Positions[InstanceIndex].position.Fl3 = XMFLOAT3 ( PosX, PosY, PosZ );

	_3DModels[IndexOfObject]->PositionsDraw[InstanceIndex].position.Fl3 = _3DModels[IndexOfObject]->Positions[InstanceIndex].position.Fl3;

	Update_3D_Object_InstancesPosBuffer ( IndexOfObject, InstanceIndex, 1 );
}


void ResourceManager::Set_3D_Object_Positon ( int IndexOfObject, int InstanceIndex, XMFLOAT3 &Pos )
{

	_3DModels[IndexOfObject]->Positions[InstanceIndex].position.Fl3 = _3DModels[IndexOfObject]->PositionsDraw[InstanceIndex].position.Fl3 = Pos;

	Update_3D_Object_InstancesPosBuffer ( IndexOfObject, InstanceIndex, 1 );

}


void ResourceManager::Set_3D_Object_Rotation ( int IndexOfObject, int InstanceIndex, XMFLOAT4 &Rotation )
{

	_3DModels[IndexOfObject]->Positions[InstanceIndex].rotation.Fl4 = _3DModels[IndexOfObject]->PositionsDraw[InstanceIndex].rotation.Fl4 = Rotation;

	Update_3D_Object_InstancesPosBuffer ( IndexOfObject, InstanceIndex, 1 );

}



void ResourceManager::InitRandInstansingPoses ( int ObjectIndex, float MaxX, float MinX, float MaxY, float MinY
											, float MaxZ, float MinZ )
{

	int c = 0;
	float ValueX = 0, ValueY = 0, ValueZ = 0;

	while ( c < _3DModels[ObjectIndex]->InstanceCount )
	{

		ValueX = ( ( float ) ( rand () % ( int ) MaxX * 2 * 10 ) / 10 ) + MinX;
		ValueY = ( ( float ) ( rand () % ( int ) MaxY * 2 * 10 ) / 10 ) + MinY;
		ValueZ = ( ( float ) ( rand () % ( int ) MaxZ * 2 * 10 ) / 10 ) + MinZ;

		_3DModels[ObjectIndex]->Positions[c].position.Fl3 = _3DModels[ObjectIndex]->PositionsDraw[c].position.Fl3 = XMFLOAT3 ( ValueX, ValueY, ValueZ );

		++c;
	}

	Update_3D_Object_InstancesPosBuffer ( ObjectIndex , -1, _3DModels[ObjectIndex]->InstanceCount );

}

void ResourceManager::InitRandInstansingRots ( int ObjectIndex )
{

	int c = 0;
	float ValueX = 0, ValueY = 0, ValueZ = 0;
	XMVECTOR TempV;

	while ( c < _3DModels[ObjectIndex]->InstanceCount )
	{

		ValueX = ( float ) ( rand () % 1000 * ( XM_2PI ) ) / 1000.0f;
		ValueY = ( float ) ( rand () % 1000 * ( XM_2PI ) ) / 1000.0f;
		ValueZ = ( float ) ( rand () % 1000 * ( XM_2PI ) ) / 1000.0f;

		TempV = XMQuaternionRotationRollPitchYaw ( ValueX, ValueY, ValueZ );

		_3DModels[ObjectIndex]->Positions[c].rotation.Vec = _3DModels[ObjectIndex]->PositionsDraw[c].rotation.Vec = TempV ;

		++c;
	}

	Update_3D_Object_InstancesPosBuffer ( ObjectIndex, -1, _3DModels[ObjectIndex]->InstanceCount );

}


void ResourceManager::Frustum_3D_Objects ( FrustumClass* Frustum )
{

	int counter = 0, instanceCounter = 0, CuledInstancesCout = 0;

	_3DModel* Temp;
	RCube_VecFloat234 TempPositions;

	size_t Size = sizeof ( XMVECTOR ) * 2;

	size_t ModelsAmount = _3DModels.size ();

	while ( counter < ModelsAmount )
	{

		Temp = _3DModels[counter];
		CuledInstancesCout = 0;
		instanceCounter = 0;

		while ( instanceCounter < Temp->InstanceCount )
		{


			TempPositions.Vec = Temp->Positions[instanceCounter].position.Vec + Temp->ObjectBox.CentralPoint.Vec;
			if ( Frustum->CheckSphere ( TempPositions.Fl3, Temp->ObjectBox.SphereRadius ) )
			{

				memcpy ( &Temp->PositionsDraw[CuledInstancesCout], &Temp->Positions[instanceCounter], Size );

				++CuledInstancesCout;

			}

			++instanceCounter;
		}

		Update_3D_Object_InstancesPosBuffer ( counter, -1, CuledInstancesCout );

		Temp->InstanceFrustumPassedAmount = CuledInstancesCout;
		
		++counter;

	}

}


void  ResourceManager::LandParamChecker (Terrain_GenInit* LandParam)
{

	if ( LandParam->MinRadius < 1 )
	{

		MessageBox (NULL, L"Мининальный радиус высот/низин меньше 1.", Error, MB_OK);
		LandParam->MinRadius = 1;

	}

	if ( LandParam->MaxRadius <= LandParam->MinRadius )
	{

		MessageBox (NULL, L"максимальный радиус гор должен дыть больше чем минимальный.", Error, MB_OK);
		LandParam->MaxRadius = LandParam->MinRadius + 1;

	}

	if ( LandParam->HeightNum < 0 )
	{

		MessageBox (NULL, L"количество высот не может быть меньше 0.", Error, MB_OK);
		LandParam->HeightNum = 0;

	}

	if ( LandParam->LowlandNum < 0 )
	{

		MessageBox (NULL, L"количество низин не может быть меньше 0.", Error, MB_OK);
		LandParam->LowlandNum = 0;

	}

	if ( LandParam->Max_Height < 0 )
	{

		MessageBox (NULL, L"максимальное значение высоты гор не может быть меньше 0.", Error, MB_OK);
		LandParam->Max_Height = 0;

	}
/*
	if ( LandParam->Min_Height < 0 )
	{

		MessageBox (NULL, L"минимальное значение размера низин не может быть меньше 0.", Error, MB_OK);
		LandParam->Min_Height = 0;

	}
*/
}


void ResourceManager::GenerateRandPoint ( Terrain* TerrainObj, PointCoord& Result )
{
	Result.x = rand () % TerrainObj->VertexInX_Row;
	Result.z = rand () % TerrainObj->VertexInZ_Col;
}


int ResourceManager::GenerateRandIntRegion ( int& Max, int& Min )
{
	int Result = Min - 1;

	while ( Result < Min )
		Result = rand () % Max;

	return Result;
}


float ResourceManager::GenerateRandFloatRegion (float& Min, float& Max)
{
	float Result = Min - 1;

	while ( Result < Min )
		Result = Min + static_cast <float> ( rand () ) / ( static_cast <float> ( RAND_MAX / ( Max - Min ) ) );

	return Result;
}


void  ResourceManager::GetCoordsByIndex (Terrain* TerrainObj, int Index, PointCoord& Result)
{
	// Its for PhysX compatibility
	//  X
	//  |
	//  ---> Z

	// Checking region error
	if ( Index > TerrainObj->Total_Vertexs_In_Terrain )
	{
		Result.z = -1;
		Result.x = -1;
	}
	else
	{
		Result.x = Index / TerrainObj->VertexInX_Row;

		Result.x > 0 ? Result.z = Index - Result.x * TerrainObj->VertexInX_Row : Result.z = Index;
	}

}


int  ResourceManager::GetIndexByCoord (Terrain* TerrainObj, PointCoord& Coord)
{
	// Checking region error
	//	if ( Coord.z < 0 || Coord.x < 0 || Coord.z > TerrainObj->VertexInZ_Col || Coord.x > TerrainObj->VertexInX_Row )
	//		return -1;
	// Its for PhysX compatibility
	//  X
	//  |
	//  ---> Z
	int Index = TerrainObj->VertexInZ_Col * Coord.x + Coord.z;

	return Index;
}


void ResourceManager::CalkAmountPointsInDir (Terrain* TerrainObj, DirectionsAmount& PointData)
{
	PointCoord Result;	// XZ position of the center vertex
	PointCoord TempCoord;
	int TempVal = 0;

	Result.x = 0;
	Result.z = 0;

	GetCoordsByIndex (TerrainObj, PointData.Index, Result);	// Get central vertex coordinates

//	PointData.CenterPos_Z = Result.z;
//	PointData.CenterPos_X = Result.x;
//	PointData.Center.Fl3 = TerrainObj->VB_Data[PointData.Index].Position;

	int PointsAmount = int(PointData.Radius / TerrainObj->QuadVertexStep); // How many points within a radius vector

	PointData.PointsAmount = PointsAmount;

	// Left points amount from center vertex
	Result.z >= PointsAmount ? PointData.Left = PointsAmount : PointData.Left = Result.z;

	// Right points amount from center vertex
	TempVal = Result.z + PointsAmount;

	TempVal > TerrainObj->VertexInZ_Col ? PointData.Right = TerrainObj->VertexInZ_Col - Result.z
		: PointData.Right = PointsAmount;

	// Down points amount from center vertex
	Result.x >= PointsAmount ? PointData.Down = PointsAmount : PointData.Down = Result.x;

	// Up points amount from center vertex
	TempVal = Result.x + PointsAmount;

	TempVal > TerrainObj->VertexInX_Row ? PointData.Up = TerrainObj->VertexInX_Row - Result.x
		: PointData.Up = PointsAmount;

	// Lowest index
	TempCoord.x = Result.x - PointData.Down;
	TempCoord.z = Result.z - PointData.Left;
	PointData.LowestIndex = GetIndexByCoord (TerrainObj, TempCoord);
	PointData.Start = TempCoord;

	// Largest index
	TempCoord.x = Result.x + PointData.Up;
	TempCoord.z = Result.z + PointData.Right;
	PointData.LowestIndex = GetIndexByCoord (TerrainObj, TempCoord);
	PointData.End = TempCoord;
}

float ResourceManager::Formula (float& Value)
{
	float temp = - static_cast <float>(1.990803164890060e01 * Value);
	return static_cast <float>( 9.857573783154564e-01 / ( 1 + 1.441927059107521e05 * pow (2.718281828459045, temp) ) );
}


void ResourceManager::MoveTerrain (Terrain* TerrainObj, DirectionsAmount& PointData, float Height)
{
	PointCoord Result;

	RCube_VecFloat234 VeetexPosition;
	RCube_VecFloat234 CenterPosition;

	VeetexPosition.Vec = { 0.0f, 0.0f, 0.0f, 0.0f };
	CenterPosition.Vec = { 0.0f, 0.0f, 0.0f, 0.0f };

	RCube_VecFloat234 Temp1, Temp2;

	float VertexTempHeight = 0.0f;
	float CenterTempHeight = 0.0f;

	int Index = 0;
	
	CenterPosition.Fl4 = TerrainObj->VB_Data[PointData.Index].Position;	// Geting Center vertex Position and Height
	CenterTempHeight = CenterPosition.Fl3.y;
	CenterPosition.Fl3.y = 0.0f;	// Exluding Height

	for ( int i = PointData.Start.x; i < PointData.End.x; ++i )
	{
		for ( int j = PointData.Start.z; j < PointData.End.z; ++j )
		{
			// Geting vertex Index
			Result.x = i;
			Result.z = j;
			Index = GetIndexByCoord (TerrainObj, Result);

			VeetexPosition.Fl4 = TerrainObj->VB_Data[Index].Position;	// Geting vertex Position and Height
			VertexTempHeight = VeetexPosition.Fl3.y;							// Store vertex Height
			
			// Checking distance from Center without Height to vertex without Height
			VeetexPosition.Fl3.y = 0.0f;	// Exluding Height

			Temp1.Vec = CenterPosition.Vec - VeetexPosition.Vec;
			Temp2.Vec = XMVector3Length (Temp1.Vec);

			float Val = abs (Temp2.Fl4.x);	// Point distance from center point

			// If point within the radius, change it height
			if ( Val < PointData.Radius )
			{
					float Temp = (PointData.Radius - Val) / 100;
					TerrainObj->VB_Data[Index].Position.y += Height * Formula (Temp);
			}
		}
	}

}


bool ResourceManager::CreateLandScapeBending (Terrain* TerrainObj, XMFLOAT2 X_Z, XMFLOAT4 HRMinMax)
{
	// Checking if any buffer was creater as IMMUTABLE for updates ( static buffers )
	_3D_Obj_Buffers*  Buffers = Get_Terrain_Buffers_By_Index (TerrainObj->TerrainBuffersIndex);
	if ( Buffers->Vertexes->GetCPU_Access() == 2 )
		return false;

	DirectionsAmount *PointData = new DirectionsAmount;

	PointCoord CenterPoint;
	float HeighValue;

	CenterPoint.x = ( int ) X_Z.x;
	CenterPoint.z = ( int ) X_Z.y;

	PointData->Radius = GenerateRandFloatRegion (HRMinMax.z, HRMinMax.w);
	PointData->Index = GetIndexByCoord (TerrainObj, CenterPoint);
	CalkAmountPointsInDir (TerrainObj, *PointData);
	HeighValue = GenerateRandFloatRegion (HRMinMax.x, HRMinMax.y);
	MoveTerrain (TerrainObj, *PointData, HeighValue);

	_3D_Obj_Buffers *Terrain_Buffers = Get_Terrain_Buffers_By_Index (TerrainObj->TerrainBuffersIndex);

	CalculateNormals (TerrainObj);

	Terrain_Buffers->Vertexes->Update (TerrainObj->VB_Data);

	Update_Terrain_Clusters_AABB (TerrainObj);	// Update for clusters frustum culling

	delete PointData;

	return true;
}


void ResourceManager::GenerateLandScape (Terrain* TerrainObj, Terrain_GenInit* InintData)
{
	DirectionsAmount *PointData = new DirectionsAmount;		// Amount of available vertex in direction from center vertex

	PointCoord CenterPoint;
	float HeighValue;

	LandParamChecker (InintData);

	// генерим низины
	for ( int count = 0; count < InintData->LowlandNum; ++count )
	{
//		float a = Local_Timer->GenerateRandFloatRegion (InintData->MaxRadius, InintData->MinRadius);
		PointData->Radius = GenerateRandFloatRegion (InintData->MinRadius, InintData->MaxRadius);
		GenerateRandPoint (TerrainObj, CenterPoint);
		PointData->Index = GetIndexByCoord (TerrainObj, CenterPoint);
		CalkAmountPointsInDir (TerrainObj, *PointData);
		HeighValue = GenerateRandFloatRegion (InintData->Min_Height, InintData->Max_Height);

		MoveTerrain (TerrainObj,*PointData, HeighValue);

	}

	// генерим высоты
	for ( int count = 0; count < InintData->HeightNum; ++count )
	{
//		float a = Local_Timer->GenerateRandFloatRegion (InintData->MaxRadius, InintData->MinRadius);
		PointData->Radius = GenerateRandFloatRegion (InintData->MaxRadius, InintData->MinRadius);
		GenerateRandPoint (TerrainObj, CenterPoint);
		PointData->Index = GetIndexByCoord (TerrainObj, CenterPoint);
		CalkAmountPointsInDir (TerrainObj, *PointData);
		HeighValue = GenerateRandFloatRegion (InintData->Max_Low, InintData->Min_Low);

		MoveTerrain (TerrainObj, *PointData, HeighValue);

	}

	_3D_Obj_Buffers *Terrain_Buffers = Get_Terrain_Buffers_By_Index (TerrainObj->TerrainBuffersIndex);
	
	CalculateNormals (TerrainObj);

//	Terrain_Buffers->Vertexes->Update (TerrainObj->VB_Data);

	Update_Terrain_Clusters_AABB ( TerrainObj );	// Update for clusters frustum culling

	delete PointData;
}


void ResourceManager::CalculateNormals ( Terrain* TerrainObj )
{
	int Index0, Index1, Index2, Index3;

	RCube_VecFloat234 Vertex0, Vertex1, Vertex2, Vertex3;
	RCube_VecFloat234 Vector1, Vector2, Vector_1, Vector_2, *Normals, Summ;

	Vertex0.Vec = { 0.0f, 0.0f, 0.0f, 0.0f };
	Vertex1.Vec = Vertex0.Vec;
	Vertex2.Vec = Vertex0.Vec;
	Vertex3.Vec = Vertex0.Vec;
	Vector1.Vec = Vertex0.Vec;
	Vector2.Vec = Vertex0.Vec;
	Vector_1.Vec = Vertex0.Vec;
	Vector_2.Vec = Vertex0.Vec;
//	Normal.Vec = Vertex0.Vec;
	Summ.Vec = Vertex0.Vec;

	int Normal_Index = 0;

	// Create a temporary array to hold the un-normalized normal vectors.
	Normals = new RCube_VecFloat234[ TerrainObj->Total_Vertexs_In_Terrain ];

	memset (Normals, 0, TerrainObj->Total_Vertexs_In_Terrain);

	// Go through all the faces in the mesh and calculate their normals.

	for (int j = 0; j < ( TerrainObj->VertexInX_Row - 1 ); j++ )
	{
		for (int i = 0; i < ( TerrainObj->VertexInZ_Col - 1 ); i++ )
		{
			// Vertex Indexes in square
			// Its for PhysX compatibility
			//			   X	
			// 3  1		   |
			// 0  2        ---> Z
			Index0 = i + j * TerrainObj->VertexInZ_Col;
			Index3 = Index0 + TerrainObj->VertexInZ_Col;
			Index1 = Index3 + 1;
			Index2 = Index0 + 1;

			Vertex0.Fl4 = TerrainObj->VB_Data[Index0].Position;
			Vertex3.Fl4 = TerrainObj->VB_Data[Index3].Position;
			Vertex1.Fl4 = TerrainObj->VB_Data[Index1].Position;
//			Vertex2.Fl3 = TerrainObj->VB_Data[Index2].Position;

			Vector1.Vec = Vertex0.Vec - Vertex3.Vec;
			Vector2.Vec = Vertex3.Vec - Vertex1.Vec;

//			Vector_1.Vec = Vertex0.Vec - Vertex1.Vec;
//			Vector_2.Vec = Vertex1.Vec - Vertex2.Vec;

//			Normal.Vec = { -1.0f, 0.0f, 0.0f, 0.0f };
			Normals[Normal_Index].Vec = XMVector3Normalize ( XMVector3Cross ( Vector1.Vec, Vector2.Vec ) );
//			Normal.Vec = XMVector3Normalize (XMVector3Cross ( Vector1.Vec, Vector2.Vec ));
//			Normal.Vec = XMVector3Normalize ( XMVector3Cross ( Vector_1.Vec, Vector_2.Vec ) );

//			Summ.Fl3 = Normal.Vec + ;

//			TerrainObj->VB_Data[Index0].Normal = Normal.Fl3;
//			TerrainObj->VB_Data[Index3].Normal = Normal.Fl3;
//			TerrainObj->VB_Data[Index1].Normal = Normal.Fl3;

//			Normal.Vec = XMVector3Normalize ( XMVector3Cross ( Vector_1.Vec, Vector_2.Vec ) );
//			TerrainObj->VB_Data[Index0].Normal = Normal.Fl3;
//			TerrainObj->VB_Data[Index1].Normal = Normal.Fl3;
//			TerrainObj->VB_Data[Index2].Normal = Normal.Fl3;

//			TerrainObj->VB_Data[Index0].Normal = Normal.Fl3;

			++Normal_Index;
		}
		// Последний вертекс в строке пропускается. Потому, что обрабатываем сразу 4 вертекса для квадрата
		++Normal_Index;
	}

	Normal_Index = 0;
	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex.
	for ( int j = 0; j < ( TerrainObj->VertexInX_Row - 1 ); ++j )
	{
		for ( int i = 0; i < ( TerrainObj->VertexInZ_Col - 1 ); ++i )
		{
			Summ.Vec = { 0.0f, 0.0f, 0.0f, 0.0f };
			
			// Vertex Indexes in square
			// 3  1
			// 0  2
			Index0 = i + j * TerrainObj->VertexInZ_Col;
			Index3 = Index0 + TerrainObj->VertexInZ_Col;
			Index1 = Index3 + 1;
			Index2 = Index0 + 1;

			Vertex0.Vec = Normals[Index0].Vec;
			Vertex3.Vec = Normals[Index3].Vec;
			Vertex1.Vec = Normals[Index1].Vec;
//			Vertex2.Vec = Normals[Index2].Vec;

			Summ.Vec = Vertex0.Vec + Vertex3.Vec + Vertex1.Vec;// +Vertex2.Vec;

			Summ.Vec /= 3.0f;

			Normals[Normal_Index].Vec = -XMVector3Normalize ( Summ.Vec );	//   !!!!!!!!!!!!!!!!!!  - 

//			Vector_1.Vec = { 0.0f, 0.0f, 0.0f, 0.0f };
//			Vector_1.Fl3 = TerrainObj->VB_Data[Normal_Index].Normal;
//			Vector_1.Vec += Normals[Normal_Index].Vec;
//			TerrainObj->VB_Data[Normal_Index].Normal = Vector_1.Fl3;

			TerrainObj->VB_Data[Normal_Index].Normal = Normals[Normal_Index].Fl3;

//  			TerrainObj->VB_Data[Normal_Index].Normal = Vertex0.Fl3;

			++Normal_Index;
		}
		// Последний вертекс в строке пропускается. Потому, что обрабатываем сразу 4 вертекса для квадрата
		++Normal_Index;
	}

	delete [] Normals;
}


void ResourceManager::Calculate_FirstVertexPos ( Terrain* TerrainObj )
{

	float HalfStep = TerrainObj->QuadVertexStep / 2;// Всё выравнивается относительно координаты 0,0,0
	int HalfROW = TerrainObj->VertexInX_Row / 2;	// количество вертексов слева и справа от x = 0
	int HalfCOL = TerrainObj->VertexInZ_Col / 2;	// количество вертексов сверху и снизу от z = 0

	// Для совпадения с PhysX
	TerrainObj->First_Vertex_Data.Fl3.x = -TerrainObj->QuadVertexStep * HalfROW - HalfStep;
	TerrainObj->First_Vertex_Data.Fl3.z = -TerrainObj->QuadVertexStep * HalfCOL - HalfStep;

}


void ResourceManager::GenerateVertexes ( Terrain* TerrainObj )
{
	int index = 0;

	float XvStep = 0.0f;
	float ZvStep = 0.0f;

	float XSetPos = 0.0f;
	float ZSetPos = 0.0f;

	float Texture_X_U = 0.0f;
	float Texture_Z_V = 0.0f;

	Calculate_FirstVertexPos ( TerrainObj );

	float Height = 0.0f;

	for ( int i = 0; i < TerrainObj->VertexInX_Row; ++i )
	{
		XSetPos = TerrainObj->First_Vertex_Data.Fl3.x + XvStep;	// Позиция по X_Row 

		for ( int j = 0; j < TerrainObj->VertexInZ_Col; ++j )
		{
			index = ( TerrainObj->VertexInZ_Col * i ) + j;	// Вычисляем индекс Vertex в массиве
			
			ZSetPos = TerrainObj->First_Vertex_Data.Fl3.z + ZvStep; // Позиция по Z_Col

//			Height  = -( float ) ( rand () % 250 );
/*			if ( (j == 0 && i == 0 ) || ( j == 4 && i == 0 ) || ( j == TerrainObj->VertexInZ_Col / 2 && (i == TerrainObj->VertexInX_Row / 2 || i == TerrainObj->VertexInX_Row / 2 + 5) ) ) //|| i == 10
				Height = 100;
			else
				Height = 0;
*/	
			// Устанавливаем позицию вертексу
			TerrainObj->VB_Data[index].Position = XMFLOAT4(XSetPos, Height, ZSetPos, 0.0f);

			// Устанавливаем текстурные координаты
			TerrainObj->VB_Data[index].TexCoord = XMFLOAT2 ( Texture_X_U, Texture_Z_V ); ;// XMFLOAT2 ( ( ( float ) i / ( float ) TerrainObj->VertexInX_Row ) * 1.0f, ( ( float ) j / ( float ) TerrainObj->VertexInZ_Col ) * 1.0f );//XMFLOAT2 ( Texture_X_U, Texture_Z_V );
			
			++Texture_Z_V;// текстурные координаты по V

			ZvStep += TerrainObj->QuadVertexStep;
		}
		Texture_Z_V = 0.0f;
		++Texture_X_U;	// текстурные координаты по U

		XvStep += TerrainObj->QuadVertexStep;
		ZvStep = 0.0f;
	}

}


bool ResourceManager::ReadTerrainFromTexture ( Terrain* TerrainObj, char* FileName )
{
	return false;
}


void ResourceManager::FrustumTerrain ( FrustumClass* Frustum, Terrain* TerrainObj )
{
	TerrainObj->PassedFrustumClasters.clear ();

	size_t Amount = TerrainObj->Clusters.size ();

	for ( size_t i = 0 ; i < Amount; ++i )
	{
		TerrainCluster* TempCluster = TerrainObj->Clusters[i];
//		if ( Frustum->CheckSphere ( TempCluster->AABB.Pos.Fl3, TempCluster->AABB.Radius ) )
		if ( Frustum->CheckRectangle (TempCluster->AABB.Pos.Fl3, TempCluster->AABB.XSize, TempCluster->AABB.YSize, TempCluster->AABB.ZSize) )
			TerrainObj->PassedFrustumClasters.emplace_back ( TempCluster );
	}

}


int ResourceManager::Delete_Terrain_Buffers ( size_t BufferIndex )
{
	size_t i = TerrainVertexBuffers.size ();
	if ( BufferIndex < i )
	{
		// Сохраняем освободившийся индекс Terrain
		UnusedTerrainsBuffersIndex.push_back ( (UINT)BufferIndex );
		// Удаляем буферы
		RCUBE_DELETE ( TerrainVertexBuffers[BufferIndex] );
		return 0;
	}

	return -1;
}


int ResourceManager::Delete_Terrain_Clusters ( Terrain* TerrainObj )
{
	// Удаляем кластеры
	size_t ClusterAmount = TerrainObj->Clusters.size ();
	for ( size_t i = 0; i < ClusterAmount; ++i )
	{
		Delete_Terrain_Buffers ( TerrainObj->Clusters[i]->ClusterBufferIndex );
		RCUBE_DELETE ( TerrainObj->Clusters[i] );
	}
	TerrainObj->Clusters.clear ();

	return 0;
}


int ResourceManager::Delete_Terrain ( size_t ObjectIndex )
{
	size_t i = Terrains.size ();

	if ( ObjectIndex < i )
	{
		Terrains[ObjectIndex]->PassedFrustumClasters.clear ();
		// Сохраняем освободившийся индекс Terrain
		UnusedTerrainsIndex.push_back ( Terrains[ObjectIndex]->Terrain_Object_Index );
		// Удаляем кластеры Terrain
		Delete_Terrain_Clusters ( Terrains[ObjectIndex] );
		// Удаляем все : буферы , текстуры , материалы модели
		Delete_Terrain_Buffers ( Terrains[ObjectIndex]->TerrainBuffersIndex );

		// Удаляем сам Terrain
		RCUBE_DELETE ( Terrains[ObjectIndex] );
		return 0;
	}
	return -1;
}


int ResourceManager::Get_New_Terrain_Index ( Terrain* TerrainObj )
{
	return TerrainObj->Terrain_Object_Index = GetObjectBuffer <vector < Terrain* >, vector<UINT>, Terrain* > ( Terrains, UnusedTerrainsIndex, TerrainObj );
}


int ResourceManager::Get_Terrain_VB_Index ( _3D_Obj_Buffers* NewBuffer )
{
	return NewBuffer->ThisBufferIndex = GetObjectBuffer <vector <_3D_Obj_Buffers* >, vector<UINT>, _3D_Obj_Buffers* > ( TerrainVertexBuffers, UnusedTerrainsBuffersIndex, NewBuffer );
}


int ResourceManager::Create_Terrain_Mesh_Buffer ( Terrain* TerrainObj, bool StaticBuffers )
{
	_3D_Obj_Buffers *Terrain_Buffers = new _3D_Obj_Buffers ();

	int ReturnIndex = Get_Terrain_VB_Index ( Terrain_Buffers );
	// Создаём буфер вертексов Terrain
	if ( StaticBuffers )
		Terrain_Buffers->Vertexes = new VertexBuffer <Vertex_Model3D> ( Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, CONST_BUFFER, TerrainObj->Total_Vertexs_In_Terrain, TerrainObj->VB_Data );
	else
	{
		Terrain_Buffers->Vertexes = new VertexBuffer <Vertex_Model3D> (Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, NO_CPU_ACCESS_BUFFER, TerrainObj->Total_Vertexs_In_Terrain, nullptr);
		// Сохраняем вертексы в Вертексный буфер
		Terrain_Buffers->Vertexes->Update (TerrainObj->VB_Data);
	}
	

	// Если Terrain НЕ рисуется кластерами
	if ( !TerrainObj->ClusterRender )
	{
		// Создаём Индексный буфер для всего Terrain
		if ( StaticBuffers )
			Terrain_Buffers->Indexes = new IndexBuffer <Index_Type> ( Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, CONST_BUFFER, TerrainObj->Total_Indexs_In_Terrain, TerrainObj->IB_Data );
		else
		{
			Terrain_Buffers->Indexes = new IndexBuffer <Index_Type> (Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, NO_CPU_ACCESS_BUFFER, TerrainObj->Total_Indexs_In_Terrain, nullptr);
			// Сохраняем индексы в Индексный буфер
			Terrain_Buffers->Indexes->Update (TerrainObj->IB_Data);
		}
	}

	TerrainObj->TerrainBuffersIndex = ReturnIndex;

	return  ReturnIndex;
}


Terrain* ResourceManager::Get_Terrain_Object_Pointer ( int ObjectIndex )
{
	size_t i = Terrains.size ();

	if ( ObjectIndex < i )
	{
		return Terrains[ObjectIndex];
	}
	return nullptr;
}


_3D_Obj_Buffers* ResourceManager::Get_Terrain_Buffers_By_Index ( int ObjectIndex )
{
	size_t i = TerrainVertexBuffers.size ();

	if ( ObjectIndex < i )
	{
		return TerrainVertexBuffers[ObjectIndex];
	}
	return nullptr;
}


// расчёт реального размера кластера по его крайним точкам RUF и LDB
void ResourceManager::CalkTerrainClusterAABB ( TerrainCluster* ClasterObject )
{
	RCube_VecFloat234 Nul_Point, Temp, Position;

	float MinY;
	float MaxY;

	Temp.Vec = ClasterObject->AABB.RUF.Vec - ClasterObject->AABB.LDB.Vec;

	MinY = ClasterObject->AABB.LDB.Fl4.y;
	MaxY = ClasterObject->AABB.RUF.Fl4.y;

	Nul_Point.Vec = XMVector3Length ( Temp.Vec );

	// Radius
	ClasterObject->AABB.Radius = Nul_Point.Fl4.x / 2;

	Temp.Vec /= 2;

	// Позиция кластера
	ClasterObject->AABB.Pos.Vec = ClasterObject->AABB.RUF.Vec - Temp.Vec;

	// Right Up Forward
	ClasterObject->AABB.RUF.Vec = Temp.Vec;

	// Left Down Backward
//	Nul_Point.Vec = { 0.0f, 0.0f, 0.0f, 0.0f };
	ClasterObject->AABB.LDB.Vec = -Temp.Vec;

	ClasterObject->AABB.RUF.Fl4.y = MaxY;
	ClasterObject->AABB.LDB.Fl4.y = MinY;


}


void ResourceManager::Calc_Next_Cluster_Start_Pos ( int& Start_X_ROW, int& Start_Z_COL, Terrain* TerrainObj )
{
	// Кластеры в строке закончились
	if ( Start_Z_COL + TerrainObj->Row_Col_VertexAmountInClasterAnyDir >= TerrainObj->VertexInZ_Col )
	{
		Start_X_ROW = Start_X_ROW + TerrainObj->Row_Col_VertexAmountInClasterAnyDir - 1;
		Start_Z_COL = 0;
	}
	// Кластеры в строке ещё есть
	else
	{
		Start_Z_COL += TerrainObj->Row_Col_VertexAmountInClasterAnyDir - 1;
	}
	return;
}


void ResourceManager::GenerateIndexes ( Terrain* TerrainObj, int GridScale )
{
	int CurrentIndex = 0;
	int Index0, Index1, Index2, Index3;

	int XDone = TerrainObj->VertexInX_Row - 1;	// Верхняя строка участвует в перечне когда цикл проходит по предпоследней строке
	int ZDone = TerrainObj->VertexInZ_Col - 1;	// Правый столбец участвует в перечне когда цикл проходит по предпоследнему столбцу

	for ( int j = 0; j < XDone; ++j )
	{
		for ( int i = 0; i < ZDone; ++i )
		{
			// Vertex Indexes in square
			// 3  1
			// 0  2
			Index0 = i + j * TerrainObj->VertexInZ_Col;
			Index3 = Index0 + TerrainObj->VertexInZ_Col;
			Index1 = Index3 + 1;
			Index2 = Index0 + 1;

			TerrainObj->IB_Data[CurrentIndex++] = Index0;
			TerrainObj->IB_Data[CurrentIndex++] = Index1;
			TerrainObj->IB_Data[CurrentIndex++] = Index2;
			TerrainObj->IB_Data[CurrentIndex++] = Index0;
			TerrainObj->IB_Data[CurrentIndex++] = Index3;
			TerrainObj->IB_Data[CurrentIndex++] = Index1;
		}
	}
}


void ResourceManager::Update_Terrain_Clusters_AABB (Terrain* TerrainObj)
{
	int Index0, Index1, Index2, Index3;

	float Min_Y = 0.0f;
	float Max_Y = 0.0f;

	float Min_X = 0.0f;
	float Max_X = 0.0f;

	float Min_Z = 0.0f;
	float Max_Z = 0.0f;

	// Количество столбцов в кластере
	int Total_Columns_In_Cluster = TerrainObj->QuadsPerCluster + 1;

	// Для наглядности разделяю переменные Col и Row
	int TotalRows_In_Claster = Total_Columns_In_Cluster;

	// Индекс последней строки в кластере
	int Last_Row_Index_In_Cluster = 0;

	// Индекс строки начального вертекса в кластере
	int StartCluster_X_ROW_Index = 0;
	// Индекс столбца начального вертекса в кластере
	int StartCluster_Z_COL_Index = 0;

	int CurrentClusterIndex = 0;	// Указатель на текущий индекс в массиве индексов для кластера

	int Column_End = 0;		// Индекс окончания строки

	// Сколько кластеров по X
	for ( int Terrin_Row = 0; Terrin_Row < TerrainObj->ClustersX_ROW; ++Terrin_Row )
	{
		// Сколько кластеров по Z
		for ( int Terrain_Col = 0; Terrain_Col < TerrainObj->ClustersZ_COL; ++Terrain_Col )
		{
			TerrainCluster* ClasterObject = TerrainObj->Clusters[CurrentClusterIndex];

			// -1 потому, что обрабатываем по 4 вертекса - для квадрата.
			Last_Row_Index_In_Cluster = StartCluster_X_ROW_Index + TotalRows_In_Claster - 1;

			Min_Y = 0.0f;
			Max_Y = 0.0f;

			// Vertex Indexes in square
			// 3  1
			// 0  2
			// Заполнение конкретными индексами индексного буфера для кластера
			for ( int Row_Index = StartCluster_X_ROW_Index; Row_Index < Last_Row_Index_In_Cluster; ++Row_Index )
			{
				// -1 потому, что обрабатываем по 4 вертекса - для квадрата.
				Column_End = StartCluster_Z_COL_Index + Total_Columns_In_Cluster - 1;

				for ( int Column_Index = StartCluster_Z_COL_Index; Column_Index < Column_End; ++Column_Index )
				{
					// определяем текущие индексы вертексов для квадрата
					Index0 = Column_Index + Row_Index * TerrainObj->VertexInZ_Col;// Columns_In_Row;
					Index3 = Index0 + TerrainObj->VertexInZ_Col;// Columns_In_Row;
					Index1 = Index3 + 1;
					Index2 = Index0 + 1;

					// Сохраняем BV для кластера
					// LDB - Left Down Backward
					if ( ( Row_Index == StartCluster_X_ROW_Index ) && ( Column_Index == StartCluster_Z_COL_Index ) )
					{
						ClasterObject->AABB.LDB.Fl4 = TerrainObj->VB_Data[Index0].Position;
						Min_X = TerrainObj->VB_Data[Index0].Position.x;
						Min_Z = TerrainObj->VB_Data[Index0].Position.z;
					}


					// RUF - Right Up Forward
					if ( ( Row_Index == Last_Row_Index_In_Cluster - 1 ) && ( Column_Index == Column_End - 1 ) )
					{
						ClasterObject->AABB.RUF.Fl4 = TerrainObj->VB_Data[Index1].Position;
						Max_X = TerrainObj->VB_Data[Index1].Position.x;
						Max_Z = TerrainObj->VB_Data[Index1].Position.z;
					}

					// Заполняем высоту и минимум для кластера
					TerrainObj->VB_Data[Index0].Position.y > Max_Y ? Max_Y = TerrainObj->VB_Data[Index0].Position.y : Max_Y;
					TerrainObj->VB_Data[Index3].Position.y > Max_Y ? Max_Y = TerrainObj->VB_Data[Index3].Position.y : Max_Y;
					TerrainObj->VB_Data[Index1].Position.y > Max_Y ? Max_Y = TerrainObj->VB_Data[Index1].Position.y : Max_Y;
					TerrainObj->VB_Data[Index2].Position.y > Max_Y ? Max_Y = TerrainObj->VB_Data[Index2].Position.y : Max_Y;

					TerrainObj->VB_Data[Index0].Position.y < Min_Y ? Min_Y = TerrainObj->VB_Data[Index0].Position.y : Min_Y;
					TerrainObj->VB_Data[Index3].Position.y < Min_Y ? Min_Y = TerrainObj->VB_Data[Index3].Position.y : Min_Y;
					TerrainObj->VB_Data[Index1].Position.y < Min_Y ? Min_Y = TerrainObj->VB_Data[Index1].Position.y : Min_Y;
					TerrainObj->VB_Data[Index2].Position.y < Min_Y ? Min_Y = TerrainObj->VB_Data[Index2].Position.y : Min_Y;
				}
			}
			// Сохраняем в AABB кластера Min и Max высоты кластера
			ClasterObject->AABB.RUF.Fl3.y = Max_Y;
			ClasterObject->AABB.LDB.Fl3.y = Min_Y;


			ClasterObject->AABB.XSize = Max_X - Min_X;
			ClasterObject->AABB.YSize = Max_Y - Min_Y;
			ClasterObject->AABB.ZSize = Max_Z - Min_Z;

			// Расчёт AABB
			CalkTerrainClusterAABB (ClasterObject);

			Calc_Next_Cluster_Start_Pos (StartCluster_X_ROW_Index, StartCluster_Z_COL_Index, TerrainObj);

			++CurrentClusterIndex;
		}

	}
}


int ResourceManager::Create_Terrain_Clusters ( Terrain* TerrainObj, bool StaticBuffers )
{
	int ReturnIndex = -1;

	int Index0, Index1, Index2, Index3;

	float Min_Y = 0.0f;
	float Max_Y = 0.0f;

	float Min_X = 0.0f;
	float Max_X = 0.0f;

	float Min_Z = 0.0f;
	float Max_Z = 0.0f;

	// Количество столбцов в кластере
	int Total_Columns_In_Cluster = TerrainObj->QuadsPerCluster + 1;
	
	// Для наглядности разделяю переменные Col и Row
	int TotalRows_In_Claster = Total_Columns_In_Cluster;

	// Индекс последней строки в кластере
	int Last_Row_Index_In_Cluster = 0;

	// Индекс строки начального вертекса в кластере
	int StartCluster_X_ROW_Index= 0;
	// Индекс столбца начального вертекса в кластере
	int StartCluster_Z_COL_Index = 0;

	int CurrentIndex = 0;	// Указатель на текущий индекс в массиве индексов для кластера

	int Column_End = 0;		// Индекс окончания строки

	// Сколько кластеров по X
	for ( int Terrin_Row = 0; Terrin_Row < TerrainObj->ClustersX_ROW; ++Terrin_Row )
	{
		// Сколько кластеров по Z
		for ( int Terrain_Col = 0; Terrain_Col < TerrainObj->ClustersZ_COL; ++Terrain_Col )
		{
			TerrainCluster* ClasterObject = new TerrainCluster;

			_3D_Obj_Buffers *Cluster_IB = new _3D_Obj_Buffers ();

			ReturnIndex = Get_Terrain_VB_Index ( Cluster_IB );

			// Сохраняем присвоенный номер IB в самом кластере
			ClasterObject->ClusterBufferIndex = ReturnIndex;
			
			Index_Type* IB_Data = new Index_Type[TerrainObj->Total_Indexs_In_Cluster];

			// -1 потому, что обрабатываем по 4 вертекса - для квадрата.
			Last_Row_Index_In_Cluster = StartCluster_X_ROW_Index + TotalRows_In_Claster - 1;

			Min_Y = 0.0f;
			Max_Y = 0.0f;

			// Vertex Indexes in square
			// 3  1
			// 0  2
			// Заполнение конкретными индексами индексного буфера для кластера
			for ( int Row_Index = StartCluster_X_ROW_Index; Row_Index < Last_Row_Index_In_Cluster; ++Row_Index )
			{
				// -1 потому, что обрабатываем по 4 вертекса - для квадрата.
				Column_End = StartCluster_Z_COL_Index + Total_Columns_In_Cluster - 1;

				for ( int Column_Index = StartCluster_Z_COL_Index; Column_Index < Column_End; ++Column_Index )
				{
					// определяем текущие индексы вертексов для квадрата
					Index0 = Column_Index + Row_Index * TerrainObj->VertexInZ_Col;// Columns_In_Row;
					Index3 = Index0 + TerrainObj->VertexInZ_Col;// Columns_In_Row;
					Index1 = Index3 + 1;
					Index2 = Index0 + 1;

					IB_Data[CurrentIndex++] = Index0;
					IB_Data[CurrentIndex++] = Index1;
					IB_Data[CurrentIndex++] = Index2;
					IB_Data[CurrentIndex++] = Index0;
					IB_Data[CurrentIndex++] = Index3;
					IB_Data[CurrentIndex++] = Index1;

					// Сохраняем BV для кластера
					// LDB - Left Down Backward
					if ( ( Row_Index == StartCluster_X_ROW_Index ) && ( Column_Index == StartCluster_Z_COL_Index ) )
					{
						ClasterObject->AABB.LDB.Fl4 = TerrainObj->VB_Data[Index0].Position;
						Min_X = TerrainObj->VB_Data[Index0].Position.x;
						Min_Z = TerrainObj->VB_Data[Index0].Position.z;
					}


					// RUF - Right Up Forward
					if ( ( Row_Index == Last_Row_Index_In_Cluster - 1 ) && ( Column_Index == Column_End - 1 ) )
					{
						ClasterObject->AABB.RUF.Fl4 = TerrainObj->VB_Data[Index1].Position;
						Max_X = TerrainObj->VB_Data[Index1].Position.x;
						Max_Z = TerrainObj->VB_Data[Index1].Position.z;
					}

					// Заполняем высоту и минимум для кластера
					TerrainObj->VB_Data[Index0].Position.y > Max_Y ? Max_Y = TerrainObj->VB_Data[Index0].Position.y : Max_Y;
					TerrainObj->VB_Data[Index3].Position.y > Max_Y ? Max_Y = TerrainObj->VB_Data[Index3].Position.y : Max_Y;
					TerrainObj->VB_Data[Index1].Position.y > Max_Y ? Max_Y = TerrainObj->VB_Data[Index1].Position.y : Max_Y;
					TerrainObj->VB_Data[Index2].Position.y > Max_Y ? Max_Y = TerrainObj->VB_Data[Index2].Position.y : Max_Y;

					TerrainObj->VB_Data[Index0].Position.y < Min_Y ? Min_Y = TerrainObj->VB_Data[Index0].Position.y : Min_Y;
					TerrainObj->VB_Data[Index3].Position.y < Min_Y ? Min_Y = TerrainObj->VB_Data[Index3].Position.y : Min_Y;
					TerrainObj->VB_Data[Index1].Position.y < Min_Y ? Min_Y = TerrainObj->VB_Data[Index1].Position.y : Min_Y;
					TerrainObj->VB_Data[Index2].Position.y < Min_Y ? Min_Y = TerrainObj->VB_Data[Index2].Position.y : Min_Y;
				}
			}
			// Сохраняем в AABB кластера Min и Max высоты кластера
			ClasterObject->AABB.RUF.Fl3.y = Max_Y;
			ClasterObject->AABB.LDB.Fl3.y = Min_Y;

			ClasterObject->AABB.XSize = Max_X - Min_X;
			ClasterObject->AABB.YSize = Max_Y - Min_Y;
			ClasterObject->AABB.ZSize = Max_Z - Min_Z;

			// Расчёт AABB
			CalkTerrainClusterAABB ( ClasterObject );

			// Создаём индексный буфер для кластера
			if ( StaticBuffers )
				Cluster_IB->Indexes = new IndexBuffer <Index_Type> (Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, CONST_BUFFER, TerrainObj->Total_Indexs_In_Cluster, IB_Data);
			else
			{
				Cluster_IB->Indexes = new IndexBuffer <Index_Type> (Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, NO_CPU_ACCESS_BUFFER, TerrainObj->Total_Indexs_In_Cluster, nullptr);
				Cluster_IB->Indexes->Update (IB_Data);
			}

			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//	Обновить массив кластеров Cluster_IB->Indexes и удалить IB_Data
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			TerrainObj->Clusters.push_back ( ClasterObject );
			RCUBE_ARR_DELETE ( IB_Data );

			CurrentIndex = 0;

			Calc_Next_Cluster_Start_Pos (StartCluster_X_ROW_Index, StartCluster_Z_COL_Index, TerrainObj );
		}

	}

	return  ReturnIndex;
}


int ResourceManager::AddTerrain ( int _TerrainTextureIndex,
				 TerrainInitData* NewTerraindata, 
				 Terrain_GenInit* TerrainData)
{
	int Result = -1;
	bool bool_result = false;

	Terrain* NewTerrain = new Terrain;

	NewTerrain->CastShadow = NewTerraindata->_CastShadow;
	NewTerrain->ClusterRender = NewTerraindata->_ClusterRender;
	NewTerrain->TerrainTextureIndex = _TerrainTextureIndex;
	NewTerrain->QuadVertexStep = NewTerraindata->_QuadVertexStep;	// Шаг между вертексами
	if ( NewTerraindata->_QuadsPerCluster > 0 )
		NewTerrain->QuadsPerCluster = NewTerraindata->_QuadsPerCluster; // Количество примитивов в одном секторе 7 * 7 , 15 * 15 , 31 * 31 , 63 * 63 , 127 * 127 , 255 * 255
	else 
	{
		MessageBox ( 0, L"Terrain QuadsPerCluster should be > 0. Setting 1 = MIN. for QuadsPerCluster", Error, MB_OK );
		NewTerrain->QuadsPerCluster = 1;
	}
	
	if ( NewTerraindata->_ClustersX_ROW > 0 )
		NewTerrain->ClustersX_ROW = NewTerraindata->_ClustersX_ROW;		// Количество секторов по X - ROW
	else
	{
		MessageBox ( 0, L"Terrain ClustersX_ROW should be > 0. Setting 1 = MIN. for ClustersX_ROW", Error, MB_OK );
		NewTerrain->ClustersX_ROW = 1;
	}
	if ( NewTerraindata->_ClustersZ_COL > 0 )
		NewTerrain->ClustersZ_COL = NewTerraindata->_ClustersZ_COL;		// Количество секторов по Z - COLUMN
	else
	{
		MessageBox ( 0, L"Terrain ClustersZ_COL should be > 0. Setting 1 = MIN. for ClustersZ_COL", Error, MB_OK );
		NewTerrain->ClustersZ_COL = 1;
	}

	NewTerrain->Row_Col_VertexAmountInClasterAnyDir = NewTerrain->QuadsPerCluster + 1;

	NewTerrain->ClustersAmount = NewTerrain->ClustersX_ROW * NewTerrain->ClustersZ_COL;	// Количество секторов в Terrain  X * Z
	
	NewTerrain->PassedFrustumClasters.reserve ( NewTerrain->ClustersAmount );			// Резервируем память под кластера для отрисовки

	if ( NewTerrain->ClustersX_ROW < 2 )
		NewTerrain->VertexInX_Row = NewTerrain->Row_Col_VertexAmountInClasterAnyDir;// Количество вертексов в ряду.
	else
		NewTerrain->VertexInX_Row = ( NewTerrain->Row_Col_VertexAmountInClasterAnyDir - 1) * NewTerrain->ClustersX_ROW + 1;
	if ( NewTerrain->ClustersZ_COL < 2 )
		NewTerrain->VertexInZ_Col = NewTerrain->Row_Col_VertexAmountInClasterAnyDir;// Количество вертексов в столбце.
	else
		NewTerrain->VertexInZ_Col = ( NewTerrain->Row_Col_VertexAmountInClasterAnyDir - 1) * NewTerrain->ClustersZ_COL + 1;
	
	NewTerrain->Total_Vertexs_In_Terrain = NewTerrain->VertexInX_Row * NewTerrain->VertexInZ_Col;	// ОБщее количество вертексов в терейне

	// Количество индексов в кластере
	NewTerrain->Total_Indexs_In_Cluster = NewTerrain->Row_Col_VertexAmountInClasterAnyDir *
										  NewTerrain->Row_Col_VertexAmountInClasterAnyDir * 6;

	NewTerrain->VB_Data = new Vertex_Model3D[NewTerrain->Total_Vertexs_In_Terrain];	// Создаём массив вертексов

	NewTerrain->Total_Indexs_In_Terrain = NewTerrain->Row_Col_VertexAmountInClasterAnyDir * NewTerrain->Row_Col_VertexAmountInClasterAnyDir * NewTerrain->ClustersAmount * 6;

	// Создаём буфер позицию для одного Instance Terrain
	NewTerrain->InstancePositionsBuffer = new VertexBuffer <PositionType> ( Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, NO_CPU_ACCESS_BUFFER, 1, nullptr );

	if ( NewTerraindata->HightFileName == nullptr )
		GenerateVertexes ( NewTerrain );	// Размещаем вертексы в соответствии с размерами и формой Terrain
	else
	{
		bool_result = ReadTerrainFromTexture ( NewTerrain, NewTerraindata->HightFileName );
		if ( !bool_result )
		{
			return -1;
		}
	}

	if ( NewTerrain->ClusterRender )
		Create_Terrain_Clusters ( NewTerrain, NewTerraindata->_StaticBuffers );
	else
	{
		NewTerrain->IB_Data = new Index_Type[NewTerrain->Total_Indexs_In_Terrain]; // Создаём массив индексов
		GenerateIndexes ( NewTerrain, 1 );	// Заполняем индексный буфер
	}

	// Generate random Terrain if needed
	if ( TerrainData )
		GenerateLandScape (NewTerrain, TerrainData);
	else
		CalculateNormals (NewTerrain);

	Create_Terrain_Mesh_Buffer ( NewTerrain, NewTerraindata->_StaticBuffers );

	Result = Get_New_Terrain_Index ( NewTerrain );

	PositionType Terrain_DefaultPosRot;

	Terrain_DefaultPosRot.position.Vec = XMVECTOR{ 0.0f, 0.0f, 0.0f, 0.0f };
	Terrain_DefaultPosRot.rotation.Vec = XMVECTOR { 0.0f, 0.0f, 0.0f, 0.0f };

	if (Result > -1 )
		Update_Terrain_Position ( Result, &Terrain_DefaultPosRot );

	return Result;
}


bool ResourceManager::Update_Terrain_Position ( int ObjectIndex, PositionType* PositionRotation )
{
	Terrain* _Terrain = Terrains[ObjectIndex];

	_Terrain->InstancePositionsBuffer->Update ( PositionRotation );

	return true;
}


int ResourceManager::Add_CubeMap ( WCHAR* TextureName )
{
	int Result = -1;

	int TempTextureIndex = 0;
	int TempShaderIndex = 0;
	int CubeMapBufferIndex = 0;

	int LatLines = 10;
	int LongLines = 10;
	int NumSphereVertices = ( ( LatLines - 2 ) * LongLines ) + 2;
	int NumSphereFaces = ( ( LatLines - 3 )*( LongLines ) * 2 ) + ( LongLines * 2 );

	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	XMMATRIX Rotationx, Rotationy;

	vector<Vertex_FlatObject> vertices ( NumSphereVertices );
	vector<Index_Type> indices ( NumSphereFaces * 3 );

	RCube_VecFloat234 currVertPos;

	TempShaderIndex = GetShaderIndexByName ( L"CubeMap" );// Получаем индекс Шейдера для отрисовки CubeMap

	if ( TempShaderIndex < 0 )
	{
		MessageBox ( 0, L"Can't get CubeMap shader by index", Error, MB_OK );
		return -1;
	}

	TempTextureIndex = InitOneTexture ( TextureName );	// Инициализируем текстуру и сохраняем её индекс
	if ( TempTextureIndex < 0 )
	{
		MessageBox ( 0, L"Can't init CubeMap texture", Error, MB_OK );
//		return -1;
		goto ERROR_END;
	}

	currVertPos.Vec = XMVectorSet ( 0.0f, 0.0f, 1.0f, 0.0f );
	vertices[0].Position = currVertPos.Fl3;

	for ( int i = 0; i < LatLines - 2; ++i )
	{
		spherePitch = float ( ( i + 1 ) * ( XM_PI / ( LatLines - 1 ) ) );
		Rotationx = XMMatrixRotationX ( spherePitch );
		for ( int j = 0; j < LongLines; ++j )
		{
			sphereYaw = float ( j * ( XM_2PI / ( LongLines ) ) );
			Rotationy = XMMatrixRotationZ ( sphereYaw );
			currVertPos.Vec = XMVector3TransformNormal ( XMVectorSet ( 0.0f, 0.0f, 1.0f, 0.0f ), ( Rotationx * Rotationy ) );
			currVertPos.Vec = XMVector3Normalize ( currVertPos.Vec );

			vertices[i*LongLines + j + 1].Position = currVertPos.Fl3;
		}
	}

	vertices[NumSphereVertices - 1].Position = { 0.0f, 0.0f, -1.0f };

	int k = 0;
	for ( int l = 0; l < LongLines - 1; ++l )
	{
		indices[k] = 0;
		indices[k + 1] = l + 1;
		indices[k + 2] = l + 2;
		k += 3;
	}

	indices[k] = 0;
	indices[k + 1] = LongLines;
	indices[k + 2] = 1;
	k += 3;

	for ( int i = 0; i < LatLines - 3; ++i )
	{
		for ( int j = 0; j < LongLines - 1; ++j )
		{
			indices[k] = i*LongLines + j + 1;
			indices[k + 1] = i*LongLines + j + 2;
			indices[k + 2] = ( i + 1 )*LongLines + j + 1;

			indices[k + 3] = ( i + 1 )*LongLines + j + 1;
			indices[k + 4] = i*LongLines + j + 2;
			indices[k + 5] = ( i + 1 )*LongLines + j + 2;

			k += 6; // next quad
		}

		indices[k] = ( i*LongLines ) + LongLines;
		indices[k + 1] = ( i*LongLines ) + 1;
		indices[k + 2] = ( ( i + 1 )*LongLines ) + LongLines;

		indices[k + 3] = ( ( i + 1 )*LongLines ) + LongLines;
		indices[k + 4] = ( i*LongLines ) + 1;
		indices[k + 5] = ( ( i + 1 )*LongLines ) + 1;

		k += 6;
	}

	for ( int l = 0; l < LongLines - 1; ++l )
	{
		indices[k] = NumSphereVertices - 1;
		indices[k + 1] = ( NumSphereVertices - 1 ) - ( l + 1 );
		indices[k + 2] = ( NumSphereVertices - 1 ) - ( l + 2 );
		k += 3;
	}

	indices[k] = NumSphereVertices - 1;
	indices[k + 1] = ( NumSphereVertices - 1 ) - LongLines;
	indices[k + 2] = NumSphereVertices - 2;

	CubeMapBufferIndex = Create_Flat_Obj_Buffers ( NO_CPU_ACCESS_BUFFER, NumSphereVertices, NumSphereFaces * 3, TexturesArr[TempTextureIndex]->SRV );
	if ( CubeMapBufferIndex < 0 )
	{
		MessageBox ( 0, L"Can't init CubeMap Buffers", Error, MB_OK );
//		return -1;
		goto ERROR_END;
	}

	FlatObjectBuffers[CubeMapBufferIndex]->FlatObjectVB->Update ( &vertices[0] );
	FlatObjectBuffers[CubeMapBufferIndex]->IndexBs->Update ( &indices[0] );

	// Добавляем CubeMap в список
	CubeMap* NewCubemap = new CubeMap;

	NewCubemap->CubeMapBuffersIndex = CubeMapBufferIndex;
	NewCubemap->CubeMapTextureIndex = TempTextureIndex;
	NewCubemap->ShaderIndex = TempShaderIndex;
	NewCubemap->NumSphereFaces = NumSphereFaces * 3;
	CubeMaps.push_back ( NewCubemap );

	Result = (int)CubeMaps.size () - 1;

	vertices.clear ();
	indices.clear ();

	return Result;

ERROR_END:
	vertices.clear ();
	indices.clear ();

	if ( TempTextureIndex > -1 )
		DeleteTexture ( TempTextureIndex );
	if ( CubeMapBufferIndex > -1 )
		Delete_Flat_ObjectBuffers ( CubeMapBufferIndex );
	return Result;
}


BB_Particles_Buffers* ResourceManager::GetEmitters_BB_Buffers_By_Index ( int BufferIndex )
{
	int Size = (int)BB_ParticleSystems_Buffers.size ();
	if ( BufferIndex < Size )
	{
		return BB_ParticleSystems_Buffers[BufferIndex];
	}
	return nullptr;
}


bool ResourceManager::Delete_Emitter_BB_Buffer ( int BufferIndex )
{

	int Size = ( int ) BB_ParticleSystems_Buffers.size ();
	
	if ( BufferIndex < Size )
	{
		Unused_BB_BuffersIndex.push_back ( ( UINT ) BufferIndex );
		RCUBE_DELETE ( BB_ParticleSystems_Buffers[BufferIndex] );
		return true;
	}

	return  false;
}


int ResourceManager::GetNewEmitter_BB_Buffers_Index ( BB_Particles_Buffers* NewBuffer )
{
	return  NewBuffer->ThisBufferIndex = GetObjectBuffer <vector < BB_Particles_Buffers* >, vector<UINT>, BB_Particles_Buffers* > ( BB_ParticleSystems_Buffers, Unused_BB_BuffersIndex, NewBuffer );
}


void ResourceManager::CreateInitial_BB_VertexBuffer ( Vertex_FlatObject* Vertices, Emitter* NewEmitter )
{
	// Initialize vertex array to zeros at first.
	memset ( Vertices, 0, ( sizeof ( Vertex_FlatObject ) * 4 ) );

	// Top left.
	Vertices[0].Position = XMFLOAT3 ( -1.0f * NewEmitter->Init_Data.InitSize, 1.0f *  NewEmitter->Init_Data.InitSize, 0.0f );  // Top left
	Vertices[0].TexCoord = XMFLOAT4 ( 0.0f, 0.0f, 0.0f, 0.0f );

	// Bottom right.
	Vertices[1].Position = XMFLOAT3 ( 1.0f *  NewEmitter->Init_Data.InitSize, -1.0f *  NewEmitter->Init_Data.InitSize, 0.0f );  // Bottom right
	Vertices[1].TexCoord = XMFLOAT4 ( 1.0f, 1.0f, 0.0f, 0.0f );

	// Bottom left.
	Vertices[2].Position = XMFLOAT3 ( -1.0f *  NewEmitter->Init_Data.InitSize, -1.0f *  NewEmitter->Init_Data.InitSize, 0.0f );  // Bottom left.
	Vertices[2].TexCoord = XMFLOAT4 ( 0.0f, 1.0f, 0.0f, 0.0f );

	// Top right.
	Vertices[3].Position = XMFLOAT3 ( 1.0f *  NewEmitter->Init_Data.InitSize, 1.0f *  NewEmitter->Init_Data.InitSize, 0.0f );  // Top right.
	Vertices[3].TexCoord = XMFLOAT4 ( 1.0f, 0.0f, 0.0f, 0.0f );

}


int ResourceManager::Create_Emitter_BB_Buffers ( int CPUAccess, UINT InstanceAmount, UINT TempTextureIndex, Emitter* NewEmitter )
{
	int Index = -1;

	BB_Particles_Buffers* _EmitterBuffers = new BB_Particles_Buffers ();

	// Init  BillBoard 4 vertexes
	Vertex_FlatObject* m_vertices = new Vertex_FlatObject[4];
	CreateInitial_BB_VertexBuffer ( m_vertices, NewEmitter );
	_EmitterBuffers->FlatObjectVB = new VertexBuffer <Vertex_FlatObject>	( Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, CPUAccess, 4, m_vertices );
	RCUBE_ARR_DELETE ( m_vertices );

	_EmitterBuffers->IndexBs	  = new IndexBuffer  <Index_Type>			( Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, CPUAccess, 6, FlatObjectIndices );
	_EmitterBuffers->InstanceVBs  = new VertexBuffer <BB_Particle_Instance> ( Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, CPUAccess, InstanceAmount, NewEmitter->BBInstances );

	_EmitterBuffers->RenderTexture = TexturesArr[TempTextureIndex]->SRV;

	Index = GetNewEmitter_BB_Buffers_Index ( _EmitterBuffers );

	return Index;
}


int ResourceManager::AddParticleSystem ( ParticleSystem* ParticleSys )
{
	return ParticleSys->ParticleSystem_Object_Index = GetObjectBuffer <vector < ParticleSystem* >, vector<UINT>, ParticleSystem* > ( ParticleSystems, UnusedParticleSystemIndex, ParticleSys );
}


bool ResourceManager::DeleteParticleSystem ( int ObjectIndex )
{
	size_t i = ParticleSystems.size ();

	if ( ObjectIndex < i )
	{
		// Сохраняем освободившийся индекс 
		UnusedParticleSystemIndex.push_back ( ParticleSystems[ObjectIndex]->ParticleSystem_Object_Index );
		ParticleSystems[ObjectIndex]->Render = false;
//		RCUBE_DELETE ( ParticleSystems[ObjectIndex]);
		return 1;
	}

	return 0;
}


int ResourceManager::AddFont ( FontClass* RCubeFont )
{
	int ReturnIndex;

	if ( !UnusedFontIndex.empty () )
	{
		ReturnIndex = UnusedFontIndex.back ();
		UnusedFontIndex.pop_back ();
		
		RCube_Font[ReturnIndex] = RCubeFont;
	}
	else
	{
		ReturnIndex = (int)RCube_Font.size ();
		RCube_Font.push_back ( RCubeFont );
	}

	return ReturnIndex;
}


void ResourceManager::DeleteLastFont ()
{
	int i = ( int )RCube_Font.size ();
	DeleteTexture ( RCube_Font[i - 1]->FontTextureIndex );
	RCUBE_DELETE ( RCube_Font[i - 1] );
	RCube_Font.pop_back ();
}


int ResourceManager::DeleteFont ( int FontIndex )
{
	int i = ( int ) RCube_Font.size ();

	if ( FontIndex < i )
	{
		UnusedFontIndex.push_back ( ( UINT ) FontIndex );
		DeleteTexture ( RCube_Font[FontIndex]->FontTextureIndex );
		RCUBE_DELETE ( RCube_Font[FontIndex] );
//		RCube_Font.erase ( RCube_Font.begin () + FontIndex );
	}

	return 0;
}


void ResourceManager::GetTextureParam ( ID3D11ShaderResourceView * Texture, XMFLOAT4& Dimention )
{
	D3D11_TEXTURE2D_DESC* InputTextureDesc = new D3D11_TEXTURE2D_DESC;
	ID3D11Texture2D* InputTexture;
	ID3D11Resource* MyRes;

	Texture->GetResource ( &MyRes );
	MyRes->QueryInterface<ID3D11Texture2D> ( &InputTexture );
	InputTexture->GetDesc ( InputTextureDesc );

	Dimention.z = ( float ) InputTextureDesc->Width;
	Dimention.w = ( float ) InputTextureDesc->Height;

	RCUBE_RELEASE ( MyRes );
	RCUBE_RELEASE ( InputTexture );
	delete InputTextureDesc;
}


int ResourceManager::Create2DTexture ( UINT Width, UINT Height )
{
	TextureData* TempData = new TextureData;
	TempData->Width = Width;
	TempData->Height = Height;
	TempData->SampleDesc.Count = 1;
	TempData->SampleDesc.Quality = 0;

	int Index = CreateTexture ( *TempData );

	if ( Index == -1 )
		MessageBox ( NULL, L"Can't create 2D Texture", Error, MB_OK );

	return Index;
}


int ResourceManager::CreateCopy2DTexture ( int CopiedTextureIndex )
{
	int NewTextureIndex = -1;

	int i = ( int ) TexturesArr.size ();
	if ( CopiedTextureIndex < i )
	{
		TextureData NewTexture;
		NewTexture.Format = TexturesArr[CopiedTextureIndex]->Format;
		NewTexture.Height = TexturesArr[CopiedTextureIndex]->Height;
		NewTexture.Width = TexturesArr[CopiedTextureIndex]->Width;
		NewTexture.Depth = 0;
		NewTexture.ArraySize = 1;
		NewTexture.MipMapLevels = 1;
		NewTexture.Type = TexturesArr[CopiedTextureIndex]->Type;
		NewTexture.SampleDesc.Count = 1;
		NewTexture.SampleDesc.Quality = 0;
		NewTexture.Usage = TexturesArr[CopiedTextureIndex]->Usage;
		NewTexture.Shared_KeyMutex = TexturesArr[CopiedTextureIndex]->Shared_KeyMutex;
		NewTexture.DepthStensil = TexturesArr[CopiedTextureIndex]->DepthStensil;
		NewTexture.GenerateMipMaps = false;
		NewTexture.RenderTarget = TexturesArr[CopiedTextureIndex]->RenderTarget;
		NewTexture.ShaderResource = TexturesArr[CopiedTextureIndex]->ShaderResource;
		NewTexture.TextureCube = TexturesArr[CopiedTextureIndex]->TextureCube;
		NewTexture.Unordered_Access = TexturesArr[CopiedTextureIndex]->Unordered_Access;

		NewTextureIndex = CreateTexture ( NewTexture );
	}
	if ( NewTextureIndex == -1 )
	{
		MessageBox ( NULL, L"Can't copy 2D Texture", Error, MB_OK );
	}
	
	Local_D3DGC->DX_deviceContext->CopyResource ( TexturesArr[NewTextureIndex]->Resource, TexturesArr[CopiedTextureIndex]->Resource );

	return NewTextureIndex;
}


COLORREF ResourceManager::GetScreenPixelColor ( POINT Position )
{
		COLORREF Color;
		bool Result = true;
		ID3D11Resource* tempResource = nullptr;
		ID3D11Texture2D* InputTexture = nullptr;
		D3D11_TEXTURE2D_DESC* InputTextureDesc = new D3D11_TEXTURE2D_DESC;
		D3D11_TEXTURE2D_DESC* desc = new D3D11_TEXTURE2D_DESC;

		// Screen - Back Buffer texture SRV used
		Local_D3DGC->BackBuffer_SRV->GetResource ( &tempResource );
		tempResource->QueryInterface<ID3D11Texture2D> ( &InputTexture );
		InputTexture->GetDesc ( InputTextureDesc );

		ZeroMemory ( desc, sizeof ( D3D11_TEXTURE2D_DESC ) );
		desc->BindFlags = 0;
		desc->Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc->Height = InputTextureDesc->Height;
		desc->Width = InputTextureDesc->Width;
		desc->ArraySize = 1;
		desc->SampleDesc.Count = 1;
		desc->SampleDesc.Quality = 0;
		desc->MipLevels = 1;
		desc->CPUAccessFlags = D3D11_CPU_ACCESS_READ; // D3D11_CPU_ACCESS_WRITE |
		desc->BindFlags = 0;
		desc->Usage = D3D11_USAGE_STAGING;
		// Сохранение возможно только для текстуры без MSAA 
		// Нужно делать ResolveSubresource

		hr = Local_D3DGC->DX_device->CreateTexture2D ( desc, 0, &Local_D3DGC->ScreenShootTexture );
		if ( FAILED ( hr ) )
		{
			MessageBox ( NULL, L"Can't create ScreenShotTexture in GetScreenPixelColor call", Error, 0 );
			Result = false;
			goto END;
		}

		// Если текстура MSAA, то делаем ResolveSubresurces
		if ( InputTextureDesc->SampleDesc.Count > 1 )
		{
			Local_D3DGC->DX_deviceContext->ResolveSubresource ( Local_D3DGC->BackBuffer_CopyResolveTexture, 0, tempResource, 0, DXGI_FORMAT_R8G8B8A8_UNORM );
			// D3DGC->BackBuffer_CopyResolveTextureSRV
			ID3D11Resource* Resource;
			Local_D3DGC->BackBuffer_CopyResolveTextureSRV->GetResource ( &Resource );
			Local_D3DGC->DX_deviceContext->CopyResource ( Local_D3DGC->ScreenShootTexture, Resource );
			RCUBE_RELEASE ( Resource );
		}
		else
		{
			Local_D3DGC->DX_deviceContext->CopyResource ( Local_D3DGC->ScreenShootTexture, tempResource );
		}

		D3D11_MAPPED_SUBRESOURCE  mapResource;
		hr = Local_D3DGC->DX_deviceContext->Map ( Local_D3DGC->ScreenShootTexture, 0, D3D11_MAP_READ, NULL, &mapResource );

/*
		union _block
		{
			byte  *data;
			unsigned int *udata;
		} block;

		UINT amount = mapResource.DepthPitch; // RowPitch * Height;
		block.data = new byte[amount];
		memcpy ( block.data, mapResource.pData, amount );
*/

		UINT* Pointer = ( UINT* ) mapResource.pData;

		// Выравнивание D3D11_MAPPED_SUBRESOURCE
		// https://gamedev.ru/code/forum/?id=242925
		// int Temp = ( mapResource.RowPitch - Local_D3DGC->ScreenWidth * sizeof ( UINT ) ) / sizeof ( UINT );
		int Temp = ( mapResource.RowPitch - Local_D3DGC->ScreenWidth * 4 ) / 4;


		int Y = Position.y * Local_D3DGC->ScreenWidth + ( Position.y * Temp );
		int X = Position.x;
		int PickPoint = Y + X;

		// Geting Pixel Color at position
		Color = Pointer[ PickPoint ];
		
		Local_D3DGC->DX_deviceContext->Unmap ( Local_D3DGC->ScreenShootTexture, 0 );

/*

		Image img;
		img.height = desc->Height;
		img.width = desc->Width;
		img.format = desc->Format;
		img.rowPitch = mapResource.RowPitch;
		img.slicePitch = mapResource.DepthPitch;
		img.pixels = block.data;

		hr = SaveToWICFile ( img, WIC_FLAGS_NONE, GetWICCodec ( WIC_CODEC_PNG ), L"1_Texture.png" );
		if ( FAILED ( hr ) )
		{
			assert ( L"Could not save Texture.png file." );
			Result = false;
		}

		delete [] block.data;
*/

END:

		delete InputTextureDesc;
		delete desc;

		RCUBE_RELEASE ( tempResource );
		RCUBE_RELEASE ( InputTexture );
		RCUBE_RELEASE ( Local_D3DGC->ScreenShootTexture );

		return Color;
}
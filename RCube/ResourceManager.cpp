#include "stdafx.h"
#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
	RCube_Sentences.reserve ( 30 );
	ComeputeShaderNames.reserve(2);
	RCube_Font.reserve ( 6 );
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
	int c = 0;
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
	
// TERRAIN
	c = 0;
	i = Terrains.size ();
	while ( c < i )
	{
		Delete_Terrain ( c );
		++c;
	}
	Terrains.clear ();
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
	c = 0;
	i = RCube_Font.size ();
	while (c < i)
	{
		RCUBE_DELETE ( RCube_Font[c] );									// Удаляем сам шрифт
		++c;
	}
	RCube_Font.clear ();

// TEXTURES
	c = 0;
	i = TexturesArr.size ();
	while ( c < i )
	{
		RCUBE_DELETE ( TexturesArr[c] );
		++c;
	}
	TexturesArr.clear ();
	UnusedTexturesIndex.clear ();		// Удаляем свободные индексы текстур

// VERTEX Shaders
	c = 0;
	while ( c < VertShaderArr.size() )
	{
		RCUBE_RELEASE( VertShaderArr[c] );
		++c;
	}
	VertShaderArr.clear();

// PIXEL Shaders
	c = 0;
	while ( c < PixShaderArr.size() )
	{
		RCUBE_RELEASE( PixShaderArr[c] );
		++c;
	}
	PixShaderArr.clear();

// BLOBS
	c = 0;
	while ( c < BlobsArr.size() )
	{
		RCUBE_RELEASE( BlobsArr[c] );
		++c;
	}
	BlobsArr.clear();

// COMPUTE Shaders
	c = 0;
	while ( c < ComeputeShaderArr.size() )
	{
		RCUBE_RELEASE( ComeputeShaderArr[c] );
		++c;
	}
	ComeputeShaderArr.clear();

// GEOMETRY Shaders
	c = 0;
	while ( c < GeomShaderArr.size() )
	{
		RCUBE_RELEASE( GeomShaderArr[c] );
		++c;
	}
	GeomShaderArr.clear();

// HULL Shaders
	c = 0;
	while ( c < HullShderArr.size() )
	{
		RCUBE_RELEASE( HullShderArr[c] );
		++c;
	}
	HullShderArr.clear();


// DOMAIN Shaders
	c = 0;
	while (c < DomainShderArr.size ())
	{
		RCUBE_RELEASE ( DomainShderArr[c] );
		++c;
	}
	DomainShderArr.clear ();

// LAYOUTS
	c = 0;
	while (c < Layouts.size ())
	{
		RCUBE_RELEASE ( Layouts[c] );
		++c;
	}
	Layouts.clear ();

// SHADERS BUNCHES
	c = 0;
	while (c < BunchArr.size())
	{
		RCUBE_ARR_DELETE( BunchArr[c].BunchName );
		++c;
	}
	BunchArr.clear();

// Compute Shaders Names
	c = 0;
	while (c < ComeputeShaderNames.size())
	{
		RCUBE_ARR_DELETE(ComeputeShaderNames[c]);
		++c;
	}
	ComeputeShaderNames.clear();

// Flat Objects
	c = 0;
	while (c < FlatObjectBuffers.size ())
	{
		RCUBE_DELETE ( FlatObjectBuffers[c] );
		++c;
	}
	FlatObjectBuffers.clear ();


	UnusedFlatObjBuffersIndex.clear ();	// Удаляем свободные индексы буферов

	Menus.clear ();					// Удаляем меню
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

	if (IsKaf[0] != ' ' && IsKaf[1] != 'K' && IsKaf[2] != 'A' && IsKaf[3] != 'F'){
		MessageBox(0, L"Данный файл не является kaf файлом.", Error, MB_OK);
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
			MessageBox( 0, L"Файл записаный в KAF поврежден.", Error, MB_OK );
			return E_FAIL;
		}
		else
		{

			Texture* NewTexture = new Texture;
			NewTexture->Resource = LoadedTextureRes;
			NewTexture->SRV = ShaderRes;

			GetNewTextureIndex ( NewTexture );

		}

		delete [] buffer;
		++c;
	}

	delete[] Buff;
	
	return S_OK;
}

HRESULT ResourceManager::InitShaders( WCHAR * kafFilename) {

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

	if (IsKaf[0] != ' ' && IsKaf[1] != 'K' && IsKaf[2] != 'A' && IsKaf[3] != 'F') {
		MessageBox(0, L"Данный файл не является kaf файлом.", L"ResourceManager Error", MB_OK);
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
			MessageBox(0, L"Неизвестная ошибка при созданиие блоба.", Error, MB_OK);
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
				MessageBox(0, L"Ошибка в создании VS шейдера.", Error, MB_OK);
				return hr;
			}

			VertShaderArr.push_back(m_vertexShader);

			++VSCounter;

			KFShadresBunch temp;

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
				MessageBox(0, L"Ошибка в создании PS шейдера.", Error, MB_OK);
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
				MessageBox(0, L"Ошибка в создании GS шейдера.", Error, MB_OK);
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
				MessageBox(0, L"Ошибка в создании HS шейдера.", Error, MB_OK);
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
				MessageBox ( 0, L"Ошибка в создании DS шейдера.", Error, MB_OK );
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
				MessageBox(0, L"Ошибка при создании CS шейдера.", Error, MB_OK);
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

	CreateLayouts ();

	ApplyLayoutsToShaderBunches ();

	delete[] TempName;
//	delete[] names;
// - Има шейдеров

	delete[] Buff;
	
	return S_OK;

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

	UINT numElements = ARRAYSIZE ( Model3D_Layout );

	ID3D10Blob* TempBlob = GetShaderBlobByName ( L"ClusteredSM" );

	hr = Local_D3DGC->DX_device->CreateInputLayout ( Model3D_Layout, numElements, TempBlob->GetBufferPointer (),
		TempBlob->GetBufferSize (), &TempLayout );
	if (FAILED ( hr ))
	{
		MessageBox ( Local_D3DGC->hwnd, L"Model3D_Layout ошибка в создании лайаута.", L"Error", MB_OK );
		return 0;
	}

	// Сохраняем Layout
	Layouts.push_back ( TempLayout );


	TempBlob = GetShaderBlobByName ( L"Font" );
	numElements = ARRAYSIZE ( FlatObject_Layout );

	hr = Local_D3DGC->DX_device->CreateInputLayout ( FlatObject_Layout, numElements, TempBlob->GetBufferPointer (),
		TempBlob->GetBufferSize (), &TempLayout );
	if (FAILED ( hr ))
	{
		MessageBox ( Local_D3DGC->hwnd, L"FlatObject_Layout ошибка в создании лайаута.", L"Error", MB_OK );
		return 0;
	}

	// Сохраняем Layout
	Layouts.push_back ( TempLayout );



	numElements = ARRAYSIZE ( FireParticles_Layout );

	TempBlob = GetShaderBlobByName ( L"FireParticle3D" );

	hr = Local_D3DGC->DX_device->CreateInputLayout ( FireParticles_Layout, numElements, TempBlob->GetBufferPointer (),
		TempBlob->GetBufferSize (), &TempLayout );
	if (FAILED ( hr ))
	{
		MessageBox ( Local_D3DGC->hwnd, L"FireParticles_Layout ошибка в создании лайаута.", L"Error", MB_OK );
		return 0;
	}

	// Сохраняем Layout
	Layouts.push_back ( TempLayout );


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
	if (BunchArr[ShadersIndex].PS != NULL)
		Local_D3DGC->DX_deviceContext->PSSetShader ( BunchArr[ShadersIndex].PS, NULL, 0 );
	else
		 Local_D3DGC->DX_deviceContext->PSSetShader( NULL, NULL, NULL );

	if (BunchArr[ShadersIndex].VS != NULL)
		Local_D3DGC->DX_deviceContext->VSSetShader ( BunchArr[ShadersIndex].VS, NULL, 0 );
	else
		Local_D3DGC->DX_deviceContext->VSSetShader( NULL, NULL, NULL );

	if (BunchArr[ShadersIndex].GS != NULL)
		Local_D3DGC->DX_deviceContext->GSSetShader ( BunchArr[ShadersIndex].GS, NULL, 0 );
	else
		Local_D3DGC->DX_deviceContext->GSSetShader ( NULL, NULL, NULL );

	if (BunchArr[ShadersIndex].DS != NULL)
		Local_D3DGC->DX_deviceContext->DSSetShader ( BunchArr[ShadersIndex].DS, NULL, 0 );
	else
		Local_D3DGC->DX_deviceContext->DSSetShader ( NULL, NULL, NULL );

	if (BunchArr[ShadersIndex].HS != NULL)
		Local_D3DGC->DX_deviceContext->HSSetShader ( BunchArr[ShadersIndex].HS, NULL, 0 );
	else
		Local_D3DGC->DX_deviceContext->HSSetShader ( NULL, NULL, NULL );

	if (BunchArr[ShadersIndex].Layout > -1)
		Local_D3DGC->DX_deviceContext->IASetInputLayout ( Layouts[(BunchArr[ShadersIndex].Layout)] );
	else
		Local_D3DGC->DX_deviceContext->IASetInputLayout ( NULL );
}

int ResourceManager::InitOneShader( WCHAR * CSOFileName) {
	std::basic_ifstream<unsigned char> Readfile(CSOFileName, std::ios::in | std::ios::binary);
	if (!Readfile) {

		MessageBox(0, L"ResourceManager. не могу найти шейдер.", Error, MB_OK);
		return hr;
	}

	int IndexShader;

	Readfile.seekg(0, std::ios::end);
	int readNowFileSize = (int)Readfile.tellg();
	Readfile.seekg(0, std::ios::beg);

	ID3D10Blob * Blob;
	
	hr = D3DCreateBlob(readNowFileSize, &Blob);
	if ( FAILED( hr ) ) {
		MessageBox(0, L"неизвестная ошибка присозданиие блоба.ResourceManager", Error, MB_OK);
		return hr;
	}

	Readfile.read(reinterpret_cast<unsigned char*>(Blob->GetBufferPointer()), readNowFileSize);

	BlobsArr.push_back(Blob);

	int Len = (int)wcslen((const wchar_t*)CSOFileName);

	if (CSOFileName[Len - 5] == 's' && CSOFileName[Len - 6] == 'v') {

		ID3D11VertexShader * m_vertexShader;

		KFShadresBunch temp;

		BunchArr.push_back(temp); // с каждого нового пиксельного шейдера добавляю новую связку шейдеров

		hr = Local_D3DGC->DX_device->CreateVertexShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &m_vertexShader);
		if ( FAILED( hr ) )
		{
			MessageBox(0, L"ResourceManager. ошибка в создании вертексного шейдера.", Error, MB_OK);
			return hr;
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
			MessageBox(0, L"ResourceManager. ошибка в создании пиксельного шейдера.", Error, MB_OK);
			return hr;
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
			MessageBox(0, L"ResourceManager. ошибка в создании геометрического шнйдера шейдера.", Error, MB_OK);
			return hr;
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
			MessageBox(0, L"ResourceManager. ошибка в создании халл шейдера.", Error, MB_OK);
			return hr;
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
			MessageBox ( 0, L"ResourceManager. ошибка в создании домаин шейдера.", Error, MB_OK );
			return hr;
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
			MessageBox(0, L"ResourceManager. ошибка в создании расчетного шейдера шейдера.", Error, MB_OK);
			return hr;
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
			MessageBox(0, L"файл поврежден.ResourceManager", Error, MB_OK);
			goto ERROR_END;
		}

	}
    if (TextureFilename[length - 3] == 'd' && TextureFilename[length - 2] == 'd' && TextureFilename[length - 1] == 's') 
	{

		hr = DirectX::CreateDDSTextureFromFile(Local_D3DGC->DX_device, TextureFilename, &NewTexture->Resource, &NewTexture->SRV, NULL);
		if ( FAILED( hr ) ) {
			MessageBox(0, L"файл поврежден.ResourceManager", Error, MB_OK);
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


	if (!Readfile) {
		MessageBox(hwnd, L"не найдено файла по задонному адресу.ResourceManager", Error, MB_OK);
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
		MessageBox(0, L"данныйй файл не является kaf файлом.ResourceManager", Error, MB_OK);
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

void  ResourceManager::Init(D3DGlobalContext * g_D3DGC) 
{

	Local_D3DGC	= g_D3DGC;
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
int ResourceManager::Create_Flat_Obj_Buffers (bool CPUAccess, UINT InstanceAmount, UINT IndexAmount = 0, DXTextureSRV* Texture = 0 )
{
//	VertexBuffer <Vertex_Model3D>* Model3DVB = nullptr;

	VertexBuffer <Vertex_FlatObject>* FlatObjectVB = nullptr;

//	VertexBuffer <PositionType>* InstanceVB = nullptr;

//	VertexBuffer <Colour_Particles_Instance>* Instances_Colour_Particles = nullptr;

	IndexBuffer < Index_Type >* TempIB = nullptr;

	Flat_Obj_Buffers *FlatObjBuffers = nullptr;

//	void * Pointer;

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

		FlatObjectVB = new VertexBuffer <Vertex_FlatObject> ( Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, CPUAccess, InstanceAmount );			// Создаём буфер вертексов FlatObject
		FlatObjBuffers->FlatObjectVB = FlatObjectVB;


	if (IndexAmount > 0)
	{
		TempIB = new IndexBuffer < Index_Type > ( Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, CPUAccess, IndexAmount );
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
			MessageBox ( Local_D3DGC->hwnd, L"Could not create 2DTexture.", Error, MB_OK );
			goto ERROR_END;
		}

		NewTexture->Resource = dynamic_cast<ID3D11Resource*>(NewTexture->Texture2D); // Resource created

	}

	if (_Data.ShaderResource)
	{
		hr = Local_D3DGC->DX_device->CreateShaderResourceView ( NewTexture->Resource, NULL, &NewTexture->SRV );
		if (FAILED ( hr ))
		{
			MessageBox ( Local_D3DGC->hwnd, L"Could not create ShaderResourceView", Error, MB_OK );
			goto ERROR_END;
		}

	}

	if (_Data.RenderTarget)
	{
		hr = Local_D3DGC->DX_device->CreateRenderTargetView ( NewTexture->Resource, NULL, &NewTexture->RTV );
		if (FAILED ( hr ))
		{
			MessageBox ( Local_D3DGC->hwnd, L"Could not create RenderTargetView", Error, MB_OK );
			goto ERROR_END;
		}

	}

	if (_Data.DepthStensil)
	{
		hr = Local_D3DGC->DX_device->CreateDepthStencilView ( NewTexture->Resource, NULL, &NewTexture->DSV );
		if (FAILED ( hr ))
		{
			MessageBox ( Local_D3DGC->hwnd, L"Could not create DepthStencilView", Error, MB_OK );
			goto ERROR_END;
		}

	}

	if (_Data.Unordered_Access )
	{
		hr = Local_D3DGC->DX_device->CreateUnorderedAccessView ( NewTexture->Resource, NULL, &NewTexture->UAV );
		if (FAILED ( hr ))
		{
			MessageBox ( Local_D3DGC->hwnd, L"Could not create UnorderedAccessView", Error, MB_OK );
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
		// Проверяем создани ли текстура как STAGING
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


void ResourceManager::BuildSentanceVertexArray ( FontClass* Font, void* vertices, char* sentence, float& drawX, float& drawY, float& RenderFontSize )
{
	Vertex_FlatObject* vertexPtr;
	int numLetters, index, i, letter;


	// Coerce the input vertices into a VertexType structure.
	vertexPtr = (Vertex_FlatObject*)vertices;

	// Get the number of letters in the sentence.
	numLetters = (int)strlen ( sentence );

	// Initialize the index to the vertex array.
	index = 0;

	// The following loop will now build the vertex and index arrays. It takes each character from the sentence and creates two triangles for it. It then maps the character from the font texture onto 
	// those two triangles using the m_Font array which has the TU texture coordinates and pixel size. Once the polygon for that character has been created it then updates the X coordinate on the 
	// screen of where to draw the next character.

	// Draw each letter onto a quad.
	for (i = 0; i<numLetters; ++i)
	{
		//  +++++++++   Добавлена поддержка ASCII символов после 127   +++++++++++
		char *Source = &sentence[i];

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
			drawX += Font->FontSize / 2;
		}
		else
		{
			_Symbol_Dim *Source = &Font->Symbols[letter - 1];
			float DYRFS = drawY - RenderFontSize;
			float DXSS = drawX + Source->Symbol_Width;
			// First triangle in quad.
			vertexPtr[index].Position = XMFLOAT3 ( drawX, drawY, 0.0f );  // Top left.
			vertexPtr[index].TexCoord = XMFLOAT2 ( Source->Start, 0.0f );
			++index;

			vertexPtr[index].Position = XMFLOAT3 ( (drawX + Source->Symbol_Width), DYRFS, 0.0f );  // Bottom right.
			vertexPtr[index].TexCoord = XMFLOAT2 ( Source->End, 1.0f );
			++index;

			vertexPtr[index].Position = XMFLOAT3 ( drawX, DYRFS, 0.0f );  // Bottom left.
			vertexPtr[index].TexCoord = XMFLOAT2 ( Source->Start, 1.0f );
			++index;

			// Second triangle in quad.
			vertexPtr[index].Position = XMFLOAT3 ( drawX, drawY, 0.0f );  // Top left.
			vertexPtr[index].TexCoord = XMFLOAT2 ( Source->Start, 0.0f );
			++index;

			vertexPtr[index].Position = XMFLOAT3 ( DXSS, drawY, 0.0f );  // Top right.
			vertexPtr[index].TexCoord = XMFLOAT2 ( Source->End, 0.0f );
			++index;

			vertexPtr[index].Position = XMFLOAT3 ( DXSS, DYRFS, 0.0f );  // Bottom right.
			vertexPtr[index].TexCoord = XMFLOAT2 ( Source->End, 1.0f );
			++index;

			// Update the x location for drawing by the size of the letter and one pixel.
			drawX = drawX + Source->Symbol_Width + 1.0f;
		}
	}

	return;
}

int ResourceManager::GetNewSentenceIndex ( SentenceType* NewSentence )	// Получить свободный индекс предложения в системе
{
	int Index = -1;
	if (!UnusedSentenceIndex.empty ())
	{
		Index = UnusedSentenceIndex.back ();
		UnusedSentenceIndex.pop_back ();
		RCube_Sentences[Index] = NewSentence;
	}
	else
	{
		RCube_Sentences.push_back ( NewSentence );
		Index = (int)(RCube_Sentences.size () - 1);
	}

	return  Index;
}

bool ResourceManager::InitializeSentence ( SentenceType* sentence, int& maxLength )
{
	// Set the maximum length of the sentence.
	sentence->MaxLength = maxLength;

	// Set the number of vertices in the vertex array.
	sentence->vertexCount = 6 * maxLength;

	sentence->VertexBufferIndex = Create_Flat_Obj_Buffers ( false, sentence->vertexCount, 0, TexturesArr[RCube_Font[sentence->FontType]->FontTextureIndex]->SRV );

	return true;
}


void ResourceManager::UpdateSentence ( int SentenceNumber, char* text, int positionX, int positionY, float RenderFontSize )
{
	int numLetters;
	Vertex_FlatObject* vertices;
	float drawX, drawY;

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
	drawX = (float)(((Local_D3DGC->ScreenWidth / 2) * -1) + positionX);
	drawY = (float)((Local_D3DGC->ScreenHeight / 2) - positionY);

	// Build the vertex array using the FontClass and the sentence information.
	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	BuildSentanceVertexArray ( RCube_Font[Source->FontType], (void*)vertices, text, drawX, drawY, RenderFontSize );

	// Copy the vertex array information into the sentence vertex buffer.
	FlatObjectBuffers[Source->VertexBufferIndex]->FlatObjectVB->Update ( vertices );

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = nullptr;

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

	Source->Colour = XMLoadFloat4 ( &data->Colour );
	Source->FontType = data->FontType;
	Source->Render = data->Render;
	Source->MaxLength = data->MaxLength;
	Source->PosX = data->PosX;
	Source->PosY = data->PosY;
	Source->ShowType = data->ShowType;
	Source->HideType = data->HideType;
	Source->Level = data->Level;
	Source->Height = RCube_Font[data->FontType]->FontHeightInPixel;

	if (data->ScrollSpeed == 0)	Source->ScrollSpeed = 30.0f;

	strcpy_s ( Source->Text, Source->MaxLength, String );

	UpdateSentence ( Num, Source->Text, data->PosX, data->PosY );

	return Num;
}


int ResourceManager::DeleteSentence ( int SentenceNumber )
{
	int i = (int)RCube_Sentences.size ();
	if (SentenceNumber < i)
	{
		Delete_Flat_ObjectBuffers ( RCube_Sentences[SentenceNumber]->VertexBufferIndex );
		UnusedSentenceIndex.push_back ( SentenceNumber );
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
			++i;
		} while (i < j);

		RCube_Sentences.clear ();
}


int ResourceManager::GetPosX ( int SentenceNumber )
{
	return RCube_Sentences[SentenceNumber]->PosX;
}


int ResourceManager::GetPosY ( int SentenceNumber )
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



int ResourceManager::Create_3D_Obj_Mesh_Buffers( bool CPUAccess, UINT InstanceAmount, UINT IndexAmount = 0 )
{
	_3D_Obj_Buffers *_3DObjBuffers = new _3D_Obj_Buffers ();
	
	int ReturnIndex = GetNew3D_Obj_Mesh_Buffer_Index( _3DObjBuffers );

	_3DObjBuffers->Vertexes = new VertexBuffer <Vertex_Model3D> ( Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, CPUAccess, InstanceAmount );			// Создаём буфер вертексов FlatObject

	if (IndexAmount > 0)
	{
		_3DObjBuffers->Indexes = new IndexBuffer < Index_Type > ( Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, CPUAccess, IndexAmount );
	}

	return ReturnIndex;

}


bool ResourceManager::LoadKFObject ( std::wstring FileName, _3DModel* New_3D_Model )
{

	RCube_VecFloat34 TempFloat3;
	RCube_VecFloat34 TempFloat3_2;

	std::basic_ifstream< UCHAR > file ( FileName, std::ios::in | std::ios::binary );
	if ( !file )
	{
		MessageBox ( NULL, L"Не удалось открыть файл для чтения.", Error, MB_OK );
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

//	_3DModel* New_3D_Model = new _3DModel;	// Создаём объект 3D модели и сохраняем его в списке 3D объектов
//	_3DModels.push_back ( New_3D_Model );

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
		NewMesh->BufferIndex = Create_3D_Obj_Mesh_Buffers ( true, NewMesh->VertexBufferSize );

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


			RCUBE_DELETE ( TextuteFile );
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
	int Index = -1;
	if ( !Unused3DModelsIndex.empty () )
	{
		Index = Unused3DModelsIndex.back ();
		Unused3DModelsIndex.pop_back ();
		_3DModels [Index] = NewModel;
	}
	else
	{
		_3DModels.push_back ( NewModel );
		Index = ( int ) ( _3DModels.size () - 1 );
	}

	NewModel->_3D_Obj_Index = Index;

	return  Index;
}


int ResourceManager::GetNew3D_Obj_Mesh_Buffer_Index ( _3D_Obj_Buffers* NewBuffer )
{
	int Index = -1;
	if ( !Unused3DModelsMeshBuffersIndex.empty () )
	{
		Index = Unused3DModelsMeshBuffersIndex.back ();
		Unused3DModelsMeshBuffersIndex.pop_back ();
		_3DModelsMeshBuffers [Index] = NewBuffer;
	}
	else
	{
		_3DModelsMeshBuffers.push_back ( NewBuffer );
		Index = ( int ) ( _3DModelsMeshBuffers.size () - 1 );
	}

	NewBuffer->ThisBufferIndex = Index;

	return  Index;
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
	Model3DPointer->InstancePositionsBuffer = new VertexBuffer <PositionType> ( Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, true, InstCount );
	
	Result = GetNew3D_Obj_Index ( Model3DPointer );

	return Result;
}


int ResourceManager::Delete_3D_Object_Meshes_Buffers ( int ObjectIndex )
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


int ResourceManager::Delete_3D_Object ( int ObjectIndex )
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
	RCube_VecFloat34 TempPositions;

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




Point ResourceManager::GenerateRandPoint ( Terrain* TerrainObj )
{
	Point Result;

	Result.x = rand () % TerrainObj->VertexInX_Row;
	Result.y = rand () % TerrainObj->VertexInZ_Col;

	return Result;
}


int ResourceManager::GenerateRandRadius ( int MaxRadius, int MinRadius )
{
	int Result = MinRadius - 1;

	while ( Result < MinRadius )
		Result = rand () % MaxRadius;

	return Result;
}


void ResourceManager::CalculateNormals ( Terrain* TerrainObj )
{
	int i, j, index1, index2, index3, index, count;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	TerrainVectorType* normals;


	// Create a temporary array to hold the un-normalized normal vectors.
	normals = new TerrainVectorType[( TerrainObj->VertexInZ_Col - 1 ) * ( TerrainObj->VertexInX_Row - 1 )];

	// Go through all the faces in the mesh and calculate their normals.
	for ( j = 0; j<( TerrainObj->VertexInZ_Col - 1 ); j++ )
	{
		for ( i = 0; i<( TerrainObj->VertexInX_Row - 1 ); i++ )
		{
			index1 = ( j * TerrainObj->VertexInX_Row ) + i;
			index2 = ( j * TerrainObj->VertexInX_Row ) + ( i + 1 );
			index3 = ( ( j + 1 ) * TerrainObj->VertexInX_Row ) + i;

			// Get three vertices from the face.
			vertex1[0] = TerrainObj->VB_Data[index1].Position.x;
			vertex1[1] = TerrainObj->VB_Data[index1].Position.y;
			vertex1[2] = TerrainObj->VB_Data[index1].Position.z;

			vertex2[0] = TerrainObj->VB_Data[index2].Position.x;
			vertex2[1] = TerrainObj->VB_Data[index2].Position.y;
			vertex2[2] = TerrainObj->VB_Data[index2].Position.z;

			vertex3[0] = TerrainObj->VB_Data[index3].Position.x;
			vertex3[1] = TerrainObj->VB_Data[index3].Position.y;
			vertex3[2] = TerrainObj->VB_Data[index3].Position.z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = ( j * ( TerrainObj->VertexInX_Row - 1 ) ) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = ( vector1[1] * vector2[2] ) - ( vector1[2] * vector2[1] );
			normals[index].y = ( vector1[2] * vector2[0] ) - ( vector1[0] * vector2[2] );
			normals[index].z = ( vector1[0] * vector2[1] ) - ( vector1[1] * vector2[0] );
		}
	}

	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex.
	for ( j = 0; j < TerrainObj->VertexInZ_Col; j++ )
	{
		for ( i = 0; i < TerrainObj->VertexInX_Row; i++ )
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Initialize the count.
			count = 0;

			// Bottom left face.
			if ( ( ( i - 1 ) >= 0 ) && ( ( j - 1 ) >= 0 ) )
			{
				index = ( ( j - 1 ) * ( TerrainObj->VertexInX_Row - 1 ) ) + ( i - 1 );

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Bottom right face.
			if ( ( i < ( TerrainObj->VertexInX_Row - 1 ) ) && ( ( j - 1 ) >= 0 ) )
			{
				index = ( ( j - 1 ) * ( TerrainObj->VertexInX_Row - 1 ) ) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper left face.
			if ( ( ( i - 1 ) >= 0 ) && ( j < ( TerrainObj->VertexInZ_Col - 1 ) ) )
			{
				index = ( j * ( TerrainObj->VertexInX_Row - 1 ) ) + ( i - 1 );

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper right face.
			if ( ( i < ( TerrainObj->VertexInX_Row - 1 ) ) && ( j < ( TerrainObj->VertexInZ_Col - 1 ) ) )
			{
				index = ( j * ( TerrainObj->VertexInX_Row - 1 ) ) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Take the average of the faces touching this vertex.
			sum[0] = ( sum[0] / ( float ) count );
			sum[1] = ( sum[1] / ( float ) count );
			sum[2] = ( sum[2] / ( float ) count );

			// Calculate the length of this normal.
			length = sqrt ( ( sum[0] * sum[0] ) + ( sum[1] * sum[1] ) + ( sum[2] * sum[2] ) );

			// Get an index to the vertex location in the height map array.
			index = ( j * TerrainObj->VertexInX_Row ) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			TerrainObj->VB_Data->Normal = XMFLOAT3 ( sum[0] / length, sum[1] / length, sum[2] / length );

		}
	}

	// Release the temporary normals.
	delete [] normals;
	normals = 0;

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
			if ( (j == 0 && i == 0 ) || ( j == 4 && i == 0 ) || ( j == TerrainObj->VertexInZ_Col / 2 && (i == TerrainObj->VertexInX_Row / 2 || i == TerrainObj->VertexInX_Row / 2 + 5) ) ) //|| i == 10
				Height = 100;
			else
				Height = 0;
	
			// Устанавливаем позицию вертексу
			TerrainObj->VB_Data[index].Position = XMFLOAT3(XSetPos, Height, ZSetPos);

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


int ResourceManager::Delete_Terrain_Buffers ( int BufferIndex )
{
	size_t i = TerrainVertexBuffers.size ();
	if ( BufferIndex < i )
	{
		// Сохраняем освободившийся индекс Terrain
		UnusedTerrainsBuffersIndex.push_back ( Terrains[BufferIndex]->TerrainBuffersIndex );
		// Удаляем буферы
		RCUBE_DELETE ( TerrainVertexBuffers[BufferIndex] );
		return 0;
	}

	return -1;
}


int ResourceManager::Delete_Terrain ( int ObjectIndex )
{
	size_t i = Terrains.size ();

	if ( ObjectIndex < i )
	{
		// Сохраняем освободившийся индекс Terrain
		UnusedTerrainsIndex.push_back ( Terrains[ObjectIndex]->Terrain_Object_Index );
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
	int Index = -1;
	if ( !UnusedTerrainsIndex.empty () )
	{
		Index = UnusedTerrainsIndex.back ();
		UnusedTerrainsIndex.pop_back ();
		Terrains[Index] = TerrainObj;
	}
	else
	{
		Terrains.push_back ( TerrainObj );
		Index = ( int ) ( Terrains.size () - 1 );
	}

	TerrainObj->Terrain_Object_Index = Index;

	return  Index;
}


int ResourceManager::Get_Terrain_VB_Index ( _3D_Obj_Buffers* NewBuffer )
{
	int Index = -1;
	if ( !UnusedTerrainsBuffersIndex.empty () )
	{
		Index = UnusedTerrainsBuffersIndex.back ();
		UnusedTerrainsBuffersIndex.pop_back ();
		TerrainVertexBuffers[Index] = NewBuffer;
	}
	else
	{
		TerrainVertexBuffers.push_back ( NewBuffer );
		Index = ( int ) ( TerrainVertexBuffers.size () - 1 );
	}

	NewBuffer->ThisBufferIndex = Index;

	return  Index;
}


int ResourceManager::Create_Terrain_Mesh_Buffer ( Terrain* TerrainObj )
{
	_3D_Obj_Buffers *Terrain_Buffers = new _3D_Obj_Buffers ();

	int ReturnIndex = Get_Terrain_VB_Index ( Terrain_Buffers );

	Terrain_Buffers->Vertexes = new VertexBuffer <Vertex_Model3D> ( Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, false , TerrainObj->TotalVertex );	// Создаём буфер вертексов Terrain

	Terrain_Buffers->Indexes = new IndexBuffer <Index_Type> ( Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, false, TerrainObj->TotalIndex );

	// Обновляем Вертексный буфер
	TerrainVertexBuffers[TerrainObj->TerrainBuffersIndex]->Vertexes->Update ( TerrainObj->VB_Data );
	// Обновляем Индексный буфер
	TerrainVertexBuffers[TerrainObj->TerrainBuffersIndex]->Indexes->Update ( TerrainObj->IB_Data );

	TerrainObj->TerrainBuffersIndex = ReturnIndex;

	return  ReturnIndex;
}


void ResourceManager::GetQuadIndex ( Terrain* TerrainObj, int UL, int DR, Point* _Result )
{
	// Проверка, чтобы индексы верхнего левого и нижнего правого вертекса квадрата были правильные
	if ( UL < 0 || DR <= UL )
	{
		_Result->x = -1;
		_Result->y = -1;
		return;
	}




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


void ResourceManager::GenerateIndexes ( Terrain* TerrainObj, int GridScale )
{
	int CurrentIndex = 0;
	int Index0, Index1, Index2, Index3;

	int ZDone = ( TerrainObj->VertexInZ_Col - 1 );
	int XDone = ( TerrainObj->VertexInX_Row - 1 );
	for ( int j = 0; j < XDone; ++j )
	{
		for ( int i = 0; i < ZDone; ++i )
		{
			// Vertex Indexes in square
			// 0  3
			// 2  1
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


int ResourceManager::Create_Terrain_Clusters ( Terrain* TerrainObj )
{

	return  0;
}


int ResourceManager::AddTerrain ( int _TerrainTextureIndex,
				 TerrainInitData* NewTerraindata )
{
	int Result = -1;
	bool bool_result = false;

	Terrain* NewTerrain = new Terrain;

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

	NewTerrain->ClustersAmount = NewTerrain->ClustersX_ROW * NewTerrain->ClustersZ_COL;				// Количество секторов в Terrain  X * Z
	NewTerrain->VertexInX_Row = ( NewTerrain->QuadsPerCluster + 1 ) * NewTerrain->ClustersX_ROW;	// Количество вертексов в ряду
	NewTerrain->VertexInZ_Col = ( NewTerrain->QuadsPerCluster + 1 ) * NewTerrain->ClustersZ_COL;	// Количество вертексов в столбце
	NewTerrain->TotalVertex = NewTerrain->VertexInX_Row * NewTerrain->VertexInZ_Col;				// ОБщее количество вертексов в терейне

	NewTerrain->VB_Data = new Vertex_Model3D[NewTerrain->TotalVertex];	// Создаём массив вертексов

	// Расчёт количества индексов
//	int TotalIndex = 6 +
//			( NewTerrain->VertexInX_Row - 2 ) * 6 +
//			( NewTerrain->VertexInZ_Col - 2 ) * 6 +
//		  ( ( NewTerrain->VertexInZ_Col - 2 ) * ( NewTerrain->VertexInX_Row - 2 ) * 6);
	NewTerrain->TotalIndex = (( NewTerrain->VertexInX_Row - 1 ) * ( NewTerrain->VertexInZ_Col - 1 )) * 6;
	NewTerrain->IB_Data = new Index_Type[NewTerrain->TotalIndex]; // Создаём массив индексов

	// Создаём буфер позицию для одного Instance Terrain
	NewTerrain->InstancePositionsBuffer = new VertexBuffer <PositionType> ( Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, false, 1 );

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
	
	GenerateIndexes ( NewTerrain, 1 );	// Заполняем индексный буфер

	CalculateNormals ( NewTerrain );

	Create_Terrain_Mesh_Buffer ( NewTerrain );

	Create_Terrain_Clusters ( NewTerrain );

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

	RCube_VecFloat34 currVertPos;

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

	CubeMapBufferIndex = Create_Flat_Obj_Buffers ( false, NumSphereVertices, NumSphereFaces * 3, TexturesArr[TempTextureIndex]->SRV );
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
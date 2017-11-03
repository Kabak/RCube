#include "stdafx.h"
#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
	ComeputeShaderNames.resize(0);
}

ResourceManager::~ResourceManager()
{
	ShutDown();
}


void ResourceManager::ShutDown()
{

// Textures
	int c = 0;
	size_t i = TexturesArr.size ();
	while ( c < i )
	{
		RCUBE_DELETE ( TexturesArr[c] );
		++c;
	}
	TexturesArr.clear ();


	c = 0;

	while ( c < VertShaderArr.size() )
	{
		RCUBE_RELEASE( VertShaderArr[c] );
		++c;
	}
	VertShaderArr.clear();

	c = 0;
	while ( c < PixShaderArr.size() )
	{
		RCUBE_RELEASE( PixShaderArr[c] );
		++c;
	}
	PixShaderArr.clear();

	c = 0;
	while ( c < BlobsArr.size() )
	{
		RCUBE_RELEASE( BlobsArr[c] );
		++c;
	}

	BlobsArr.clear();

	c = 0;
	while ( c < ComeputeShaderArr.size() )
	{

		RCUBE_RELEASE( ComeputeShaderArr[c] );

		++c;
	}
	ComeputeShaderArr.clear();

	c = 0;
	while ( c < GeomShaderArr.size() )
	{

		RCUBE_RELEASE( GeomShaderArr[c] );

		++c;
	}
	GeomShaderArr.clear();

	c = 0;
	while ( c < HullShderArr.size() )
	{

		RCUBE_RELEASE( HullShderArr[c] );

		++c;
	}
	HullShderArr.clear();

	// Удаляем все DomainShaders
	c = 0;
	while (c < DomainShderArr.size ())
	{

		RCUBE_RELEASE ( DomainShderArr[c] );

		++c;
	}
	DomainShderArr.clear ();

	// Удаляем все Layout
	c = 0;
	while (c < Layouts.size ())
	{
		RCUBE_RELEASE ( Layouts[c] );
		++c;
	}
	Layouts.clear ();

	// Удаляем все связки шейдеров
	c = 0;
	while (c < BunchArr.size())
	{
		RCUBE_ARR_DELETE( BunchArr[c].BunchName );
		++c;
	}
	BunchArr.clear();

	c = 0;
	while (c < ComeputeShaderNames.size())
	{
		RCUBE_ARR_DELETE(ComeputeShaderNames[c]);
		++c;
	}
	ComeputeShaderNames.clear();

	c = 0;
	while (c < FlatObjectBuffers.size ())
	{
		RCUBE_DELETE ( FlatObjectBuffers[c] );
		++c;
	}
	FlatObjectBuffers.clear ();


	UnUsedBuffersIndex.clear ();	// Удаляем свободные индексы буферов
	UnusedTextures.clear ();		// Удаляем свободные индексы текстур
	Menus.clear ();					// Удаляем меню
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
			
			TexturesArr.push_back ( NewTexture );
			int Index = (int)(TexturesArr.size () - 1);
			TexturesArr[Index]->Index = Index;	// Сохраняем собственный индекс в своём теле

		}

		delete [] buffer;
		++c;
	}

	delete[] Buff;
	//Readfile.close();
	
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

	ID3D11ShaderResourceView * ShaderResource;
	ID3D11Resource * LoadedTextureResource;

	length = (UINT)wcslen(TextureFilename);

	if ((TextureFilename[length - 3] == 'p' && TextureFilename[length - 2] == 'n' && TextureFilename[length - 1] == 'g') ||
		(TextureFilename[length - 3] == 'j' && TextureFilename[length - 2] == 'p' && TextureFilename[length - 1] == 'g') ||
		(TextureFilename[length - 3] == 'b' && TextureFilename[length - 2] == 'm' && TextureFilename[length - 1] == 'p')) 
	{


		hr = DirectX::CreateWICTextureFromFile(Local_D3DGC->DX_device, TextureFilename, &LoadedTextureResource, &ShaderResource, NULL);
		if ( FAILED( hr ) ) {
			MessageBox(0, L"файл поврежден.ResourceManager", Error, MB_OK);
			return hr;
		}

		Texture* NewTexture = new Texture;
		NewTexture->Resource = LoadedTextureResource;
		NewTexture->SRV = ShaderResource;

		TexturesArr.push_back ( NewTexture );
		Result = (int)(TexturesArr.size () - 1);
		TexturesArr[Result]->Index = Result;	// Сохраняем собственный индекс в своём теле

	}
    if (TextureFilename[length - 3] == 'd' && TextureFilename[length - 2] == 'd' && TextureFilename[length - 1] == 's') 
	{


		hr = DirectX::CreateDDSTextureFromFile(Local_D3DGC->DX_device, TextureFilename, &LoadedTextureResource, &ShaderResource, NULL);
		if ( FAILED( hr ) ) {
			MessageBox(0, L"файл поврежден.ResourceManager", Error, MB_OK);
			return hr;
		}
		
		Texture* NewTexture = new Texture;
		NewTexture->Resource = LoadedTextureResource;
		NewTexture->SRV = ShaderResource;

		TexturesArr.push_back ( NewTexture );
		Result = (int)(TexturesArr.size () - 1);
		TexturesArr[Result]->Index = Result;	// Сохраняем собственный индекс в своём теле

	}

	return Result;

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

	VertexBuffer <PositionType>* InstanceVB = nullptr;

//	VertexBuffer <Colour_Particles_Instance>* Instances_Colour_Particles = nullptr;

	IndexBuffer < Index_Type >* TempIB = nullptr;

	Flat_Obj_Buffers *FlatObjBuffers = nullptr;

//	void * Pointer;

	// https://habrahabr.ru/company/xakep/blog/257891/
	// http://www.quizful.net/post/Inheritance-in-C++
	// https://habrahabr.ru/post/106294/  приведение типов
	// http://www.cplusplus.com/reference/vector/vector/ VECTORS
	int ReturnIndex;

	if (!UnUsedBuffersIndex.empty ())
	{
		FlatObjBuffers = new Flat_Obj_Buffers ();
		ReturnIndex = UnUsedBuffersIndex.back();
		UnUsedBuffersIndex.pop_back ();
		FlatObjectBuffers[ReturnIndex] = FlatObjBuffers;
	}
	else
	{
		ReturnIndex = (int)FlatObjectBuffers.size ();
		FlatObjBuffers = new Flat_Obj_Buffers ();
		FlatObjectBuffers.push_back ( FlatObjBuffers );
	}

		FlatObjectVB = new VertexBuffer <Vertex_FlatObject> ( Local_D3DGC->DX_device, Local_D3DGC->DX_deviceContext, CPUAccess, 4 );			// Создаём буфер вертексов FlatObject
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
		UnUsedBuffersIndex.push_back ( Index );
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
		UnusedTextures.push_back ( Index );
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


int ResourceManager::CreateTexture ( TextureData _Data )
{
	int ReturnIndex;

	ID3D11Texture2D* TempTexture2D = nullptr;
	ID3D11ShaderResourceView* TempSRV = nullptr;
	ID3D11DepthStencilView* TempDSV = nullptr;
	ID3D11RenderTargetView*	TempRTV = nullptr;
	ID3D11UnorderedAccessView* TempUAV = nullptr;

	Texture* NewTexture = new Texture;

	if (_Data.Type == _2D)
	{

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
		
		_Data.ShaderResource	? sharedTexDesc.BindFlags | D3D11_BIND_SHADER_RESOURCE : sharedTexDesc.BindFlags;
		_Data.DepthStensil		? sharedTexDesc.BindFlags | D3D11_BIND_DEPTH_STENCIL   : sharedTexDesc.BindFlags;
		_Data.RenderTarget		? sharedTexDesc.BindFlags | D3D11_BIND_RENDER_TARGET   : sharedTexDesc.BindFlags;
		_Data.Unordered_Access	? sharedTexDesc.BindFlags | D3D11_BIND_UNORDERED_ACCESS: sharedTexDesc.BindFlags;

		_Data.Shared_KeyMutex	? sharedTexDesc.MiscFlags | D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX	: sharedTexDesc.MiscFlags;
		_Data.TextureCube		? sharedTexDesc.MiscFlags | D3D11_RESOURCE_MISC_TEXTURECUBE			: sharedTexDesc.MiscFlags;
		_Data.GenerateMipMaps	? sharedTexDesc.MiscFlags | D3D11_RESOURCE_MISC_GENERATE_MIPS		: sharedTexDesc.MiscFlags;

		hr = Local_D3DGC->DX_device->CreateTexture2D ( &sharedTexDesc, NULL, &TempTexture2D );
		if (FAILED ( hr ))
		{
			MessageBox ( Local_D3DGC->hwnd, L"Could not create 2DTexture.", Error, MB_OK );
			goto ERROR_END;
		}

		NewTexture->Texture2D = TempTexture2D;  // 2Dtexture created
		NewTexture->Resource = dynamic_cast<ID3D11Resource*>(TempTexture2D); // Resource created

	}

	if (_Data.ShaderResource)
	{
		hr = Local_D3DGC->DX_device->CreateShaderResourceView ( NewTexture->Resource, NULL, &TempSRV );
		if (FAILED ( hr ))
		{
			MessageBox ( Local_D3DGC->hwnd, L"Could not create ShaderResourceView", Error, MB_OK );
			goto ERROR_END;
		}

		NewTexture->SRV = TempSRV; // Shader Resource View created
	}

	if (_Data.RenderTarget)
	{
		hr = Local_D3DGC->DX_device->CreateRenderTargetView ( NewTexture->Resource, NULL, &TempRTV );
		if (FAILED ( hr ))
		{
			MessageBox ( Local_D3DGC->hwnd, L"Could not create RenderTargetView", Error, MB_OK );
			goto ERROR_END;
		}

		NewTexture->RTV = TempRTV; // Shader Resource View created
	}

	if (_Data.DepthStensil)
	{
		hr = Local_D3DGC->DX_device->CreateDepthStencilView ( NewTexture->Resource, NULL, &TempDSV );
		if (FAILED ( hr ))
		{
			MessageBox ( Local_D3DGC->hwnd, L"Could not create DepthStencilView", Error, MB_OK );
			goto ERROR_END;
		}

		NewTexture->DSV = TempDSV; // Shader Resource View created
	}

	if (_Data.Unordered_Access )
	{
		hr = Local_D3DGC->DX_device->CreateUnorderedAccessView ( NewTexture->Resource, NULL, &TempUAV );
		if (FAILED ( hr ))
		{
			MessageBox ( Local_D3DGC->hwnd, L"Could not create UnorderedAccessView", Error, MB_OK );
			goto ERROR_END;
		}

		NewTexture->UAV = TempUAV; // Shader Resource View created
	}

// Сохраняем данные созданной текстуры в её описании
	NewTexture->DepthStensil	= _Data.DepthStensil;
	NewTexture->Format			= _Data.Format;
	NewTexture->Height			= _Data.Height;
	NewTexture->Width			= _Data.Width;
	NewTexture->RenderTarget	= _Data.RenderTarget;
	NewTexture->Shared_KeyMutex = _Data.Shared_KeyMutex;
	NewTexture->TextureCube		= _Data.TextureCube;
	NewTexture->Type			= _Data.Type;
	NewTexture->Unordered_Access= _Data.Unordered_Access;
	NewTexture->Usage			= _Data.Usage;

// Сохраняем созданные ресурсы текстуры в ощем списке текстур и вызвращяем её индекс
	if (!UnusedTextures.empty ())
	{
		ReturnIndex = UnusedTextures.back ();
		UnusedTextures.pop_back ();
		TexturesArr[ReturnIndex] = NewTexture;
	}
	else
	{
		ReturnIndex = (int)UnusedTextures.size ();
		TexturesArr.push_back ( NewTexture );
	}

	NewTexture->Index = ReturnIndex;

return ReturnIndex;

ERROR_END:
	RCUBE_DELETE  ( NewTexture );
	RCUBE_RELEASE ( TempTexture2D );
	RCUBE_RELEASE ( TempSRV );
	RCUBE_RELEASE ( TempRTV );
	RCUBE_RELEASE ( TempDSV );
	RCUBE_RELEASE ( TempUAV );

return -1;
}



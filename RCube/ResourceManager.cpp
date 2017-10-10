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
	int c = 0;
	while ( c < TexturesNum )
	{
		RCUBE_RELEASE( ShaderResourceArr[c] );
		RCUBE_RELEASE( ResourceArr[c] );
		++c;
	}
	ShaderResourceArr.clear();
	ResourceArr.clear();

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

	// ������� ��� DomainShaders
	c = 0;
	while (c < DomainShderArr.size ())
	{

		RCUBE_RELEASE ( DomainShderArr[c] );

		++c;
	}
	DomainShderArr.clear ();

	// ������� ��� Layout
	c = 0;
	while (c < Layouts.size ())
	{
		RCUBE_RELEASE ( Layouts[c] );
		++c;
	}
	Layouts.clear ();

	// ������� ��� ������ ��������
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
	
}


HRESULT ResourceManager::InitTextures(WCHAR * kafFilename){

	unsigned char * Buff = nullptr , * temp;
	decodeFile(&Buff, kafFilename  , active_hwnd);
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
		MessageBox(active_hwnd, L"������ ���� �� �������� kaf ������.", Error, MB_OK);
		return E_FAIL;
	}

	
	memcpy((unsigned char*)&DescStart, temp, sizeof(int));
	temp += sizeof(int);

	memcpy((unsigned char*)&TexturesNum, temp, sizeof(int));
	temp += sizeof(int);

	ID3D11ShaderResourceView * ShaderRes;
	ID3D11Resource * LoadedTextureRes;

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

//			hr = DirectX::CreateWICTextureFromMemory(active_D3DGC->D11_device, active_D3DGC->D11_deviceContext, &buffer[0], (size_t)readFileSize, &LoadedTextureRes
//			 , &ShaderRes, NULL);
// https://github.com/Microsoft/DirectXTK/wiki/WICTextureLoader
			hr = DirectX::CreateWICTextureFromMemoryEx( 
			active_D3DGC->D11_device , 
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

			hr = DirectX::CreateDDSTextureFromMemory(active_D3DGC->D11_device, active_D3DGC->D11_deviceContext, &buffer[0], (size_t)readFileSize, &LoadedTextureRes
				, &ShaderRes, NULL);
		}

		if ( FAILED( hr ) )
		{
			MessageBox( active_hwnd, L"���� ��������� � KAF ���������.", Error, MB_OK );
			return E_FAIL;
		}
		else
		{
			ShaderResourceArr.push_back( ShaderRes );
			ResourceArr.push_back( LoadedTextureRes );
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
	decodeFile(&Buff, kafFilename, active_hwnd);
	unsigned char IsKaf[4];
	unsigned char ObjFormat[3];
	int DescStart;	// ����� ��� ������
	int c = 0, readFileSize = 0;

	int VSCounter = 0;
	int CSCounter = 0;

	temp = Buff;
	memcpy(IsKaf, temp, 4);
	temp += 4;

	if (IsKaf[0] != ' ' && IsKaf[1] != 'K' && IsKaf[2] != 'A' && IsKaf[3] != 'F') {
		MessageBox(active_hwnd, L"������ ���� �� �������� kaf ������.", L"ResourceManager Error", MB_OK);
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
			MessageBox(active_hwnd, L"����������� ������ ��� ��������� �����.", Error, MB_OK);
			return hr;
		}

		memcpy(reinterpret_cast<unsigned char*>(Blob->GetBufferPointer()), temp, readFileSize);
		temp += readFileSize;

		BlobsArr.push_back(Blob);


		if (ObjFormat[0] == 'v' && ObjFormat[1] == 's' && ObjFormat[2] == ' ') 
		{
			// Create the vertex shader from the buffer.
			hr = active_D3DGC->D11_device->CreateVertexShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &m_vertexShader);
			if (FAILED( hr ))
			{
				MessageBox(active_hwnd, L"������ � �������� VS �������.", Error, MB_OK);
				return hr;
			}

			VertShaderArr.push_back(m_vertexShader);

			++VSCounter;

			KFShadresBunch temp;

			BunchArr.push_back(temp); // � ������� ������ ���������� ������� �������� ����� ������ ��������

			BunchArr[BunchArr.size() - 1].VS = m_vertexShader;

			VertexShadreBlobIndexes.push_back((int)BlobsArr.size() - 1);
		}
		if (ObjFormat[0] == 'p' && ObjFormat[1] == 's' && ObjFormat[2] == ' ') 
		{
			// Create the vertex shader from the buffer.
			hr = active_D3DGC->D11_device->CreatePixelShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &m_pixelShader);
			if (FAILED( hr ))
			{
				MessageBox(active_hwnd, L"������ � �������� PS �������.", Error, MB_OK);
				return hr;
			}

			PixShaderArr.push_back(m_pixelShader);

			BunchArr[BunchArr.size() - 1].PS = m_pixelShader;
		}

		if (ObjFormat[0] == 'g' && ObjFormat[1] == 's' && ObjFormat[2] == ' ') 
		{
			// Create the vertex shader from the buffer.
			hr = active_D3DGC->D11_device->CreateGeometryShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &m_geomShader);
			if (FAILED( hr ))
			{
				MessageBox(active_hwnd, L"������ � �������� GS �������.", Error, MB_OK);
				return hr;
			}

			GeomShaderArr.push_back(m_geomShader);


			BunchArr[BunchArr.size() - 1].GS = m_geomShader;
		}

		if (ObjFormat[0] == 'h' && ObjFormat[1] == 's' && ObjFormat[2] == ' ') 
		{
			// Create the vertex shader from the buffer.
			hr = active_D3DGC->D11_device->CreateHullShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &m_hullShader);
			if (FAILED( hr ))
			{
				MessageBox(active_hwnd, L"������ � �������� HS �������.", Error, MB_OK);
				return hr;
			}

			HullShderArr.push_back(m_hullShader);


			BunchArr[BunchArr.size() - 1].HS = m_hullShader;
		}

		if (ObjFormat[0] == 'd' && ObjFormat[1] == 's' && ObjFormat[2] == ' ')
		{
			// Create the vertex shader from the buffer.
			hr = active_D3DGC->D11_device->CreateDomainShader ( Blob->GetBufferPointer (), Blob->GetBufferSize (), NULL, &m_domainShader );
			if (FAILED ( hr ))
			{
				MessageBox ( active_hwnd, L"������ � �������� DS �������.", Error, MB_OK );
				return hr;
			}

			DomainShderArr.push_back ( m_domainShader );


			BunchArr[BunchArr.size () - 1].DS = m_domainShader;
		}

		if (ObjFormat[0] == 'c' && ObjFormat[1] == 's' && ObjFormat[2] == ' ') 
		{
			// Create the vertex shader from the buffer.
			hr = active_D3DGC->D11_device->CreateComputeShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &m_ComputeShader );
			if (FAILED( hr ))
			{
				MessageBox(active_hwnd, L"������ ��� �������� CS �������.", Error, MB_OK);
				return hr;
			}

			ComeputeShaderArr.push_back( m_ComputeShader );

			++CSCounter;
		}

		++c;
	}

// + ����� ��������
	int NamesAmount = VSCounter + CSCounter; // ���������� ������ ����� ��������
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
		//  !!!!   ����� , ���� ���������� ����� ������ 3 ��������
		NamesStart += SizeOfWchar_t * 4; // ���������� 3 ������� ������ : ".123"

		TempName[SymbolCount - 3] = NULL; // �������� _vs � _cs

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
// - ��� ��������

	delete[] Buff;
	
	return S_OK;

}


void ResourceManager::ApplyLayoutsToShaderBunches ()
{
	size_t j = BunchArr.size ();
	for (size_t i = 0; i < j; ++i)
	{
		if (ShaderLayoutBind[i] > -1)
			BunchArr[i].Layout = Layouts[ShaderLayoutBind[i]];
		else
			BunchArr[i].Layout = nullptr; // ���� ��� Layout
	}
}


bool ResourceManager::CreateLayouts ()
{
	ID3D11InputLayout* TempLayout;

	UINT numElements = ARRAYSIZE ( Model3D_Layout );

	ID3D10Blob* TempBlob = GetShaderBlobByName ( L"ClusteredSM" );

	hr = active_D3DGC->D11_device->CreateInputLayout ( Model3D_Layout, numElements, TempBlob->GetBufferPointer (),
		TempBlob->GetBufferSize (), &TempLayout );
	if (FAILED ( hr ))
	{
		MessageBox ( active_D3DGC->hwnd, L"CreateLayouts N0 ������ � �������� �������.", L"Error", MB_OK );
		return 0;
	}

	// ��������� Layout
	Layouts.push_back ( TempLayout );


	TempBlob = GetShaderBlobByName ( L"Font" );
	numElements = ARRAYSIZE ( FlatObject_Layout );

	hr = active_D3DGC->D11_device->CreateInputLayout ( Model3D_Layout, numElements, TempBlob->GetBufferPointer (),
		TempBlob->GetBufferSize (), &TempLayout );
	if (FAILED ( hr ))
	{
		MessageBox ( active_D3DGC->hwnd, L"CreateLayouts N1 ������ � �������� �������.", L"Error", MB_OK );
		return 0;
	}

	// ��������� Layout
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


ID3D10Blob* ResourceManager::GetShaderBlobByName ( wchar_t* Name )
{
	int Bunchsize = (int)BunchArr.size ();
	for (int i = 0; i < Bunchsize; ++i)
	{
		if (!wcscmp ( Name, BunchArr[i].BunchName ))
			return BlobsArr[i*2]; // ���������� ������ VS �������
	}
	return nullptr;
}


void ResourceManager::SetActiveShadersInProgramm(int ShadersIndex) {

	if(BunchArr[ShadersIndex].PS != nullptr)
		active_D3DGC->D11_deviceContext->PSSetShader(BunchArr[ShadersIndex].PS, NULL, 0);

	if(BunchArr[ShadersIndex].VS != nullptr)
		active_D3DGC->D11_deviceContext->VSSetShader(BunchArr[ShadersIndex].VS, NULL, 0);

	if (BunchArr[ShadersIndex].GS != nullptr)
		active_D3DGC->D11_deviceContext->GSSetShader(BunchArr[ShadersIndex].GS, NULL, 0);

	if (BunchArr[ShadersIndex].DS != nullptr)
		active_D3DGC->D11_deviceContext->DSSetShader(BunchArr[ShadersIndex].DS, NULL, 0 );

	if (BunchArr[ShadersIndex].HS != nullptr)
		active_D3DGC->D11_deviceContext->HSSetShader(BunchArr[ShadersIndex].HS, NULL, 0);

	if (BunchArr[ShadersIndex].Layout != nullptr)
		active_D3DGC->D11_deviceContext->IASetInputLayout ( BunchArr[ShadersIndex].Layout );
/*
	if (BunchArr[ShadersIndex].CS != nullptr)
		active_D3DGC->D11_deviceContext->CSSetShader(BunchArr[ShadersIndex].CS, NULL, 0);
*/
}

int ResourceManager::InitOneShader( WCHAR * CSOFileName) {
	std::basic_ifstream<unsigned char> Readfile(CSOFileName, std::ios::in | std::ios::binary);
	if (!Readfile) {

		MessageBox(active_hwnd, L"ResourceManager. �� ���� ����� ������.", Error, MB_OK);
		return hr;
	}

	int IndexShader;

	Readfile.seekg(0, std::ios::end);
	int readNowFileSize = (int)Readfile.tellg();
	Readfile.seekg(0, std::ios::beg);

	ID3D10Blob * Blob;
	
//	ShadersForRenderDecs TimeDesc;

	hr = D3DCreateBlob(readNowFileSize, &Blob);
	if ( FAILED( hr ) ) {
		MessageBox(active_hwnd, L"����������� ������ ������������ �����.ResourceManager", Error, MB_OK);
		return hr;
	}

	Readfile.read(reinterpret_cast<unsigned char*>(Blob->GetBufferPointer()), readNowFileSize);

	BlobsArr.push_back(Blob);

	int Len = (int)wcslen((const wchar_t*)CSOFileName);

	if (CSOFileName[Len - 5] == 's' && CSOFileName[Len - 6] == 'v') {

		ID3D11VertexShader * m_vertexShader;

		KFShadresBunch temp;

		BunchArr.push_back(temp); // � ������� ������ ����������� ������� �������� ����� ������ ��������

		hr = active_D3DGC->D11_device->CreateVertexShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &m_vertexShader);
		if ( FAILED( hr ) )
		{
			MessageBox(active_hwnd, L"ResourceManager. ������ � �������� ����������� �������.", Error, MB_OK);
			return hr;
		}

		VertShaderArr.push_back(m_vertexShader);
		BunchArr[BunchArr.size() - 1].VS = m_vertexShader;  // �������� ���������� ���� ������ 

		IndexShader = (int)VertShaderArr.size();

	}

	if (CSOFileName[Len - 5] == 's' && CSOFileName[Len - 6] == 'p') 
	{

		ID3D11PixelShader * m_pixelShader;

		hr = active_D3DGC->D11_device->CreatePixelShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &m_pixelShader);
		if ( FAILED( hr ) )
		{
			MessageBox(active_hwnd, L"ResourceManager. ������ � �������� ����������� �������.", Error, MB_OK);
			return hr;
		}

		PixShaderArr.push_back(m_pixelShader);
		BunchArr[BunchArr.size() - 1].PS = m_pixelShader;

		IndexShader = (int)PixShaderArr.size();

	}

	if (CSOFileName[Len - 5] == 's' && CSOFileName[Len - 6] == 'g') 
	{

		ID3D11GeometryShader * m_geomShader;

		hr = active_D3DGC->D11_device->CreateGeometryShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &m_geomShader);
		if ( FAILED( hr ) )
		{
			MessageBox(active_hwnd, L"ResourceManager. ������ � �������� ��������������� ������� �������.", Error, MB_OK);
			return hr;
		}

		GeomShaderArr.push_back(m_geomShader);
		BunchArr[BunchArr.size() - 1].GS = m_geomShader;

		IndexShader = (int)GeomShaderArr.size();

	}

	if (CSOFileName[Len - 5] == 's' && CSOFileName[Len - 6] == 'h') 
	{

		ID3D11HullShader * m_hullShader;

		hr = active_D3DGC->D11_device->CreateHullShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &m_hullShader);
		if ( FAILED( hr ) )
		{
			MessageBox(active_hwnd, L"ResourceManager. ������ � �������� ���� �������.", Error, MB_OK);
			return hr;
		}

		HullShderArr.push_back(m_hullShader);
		BunchArr[BunchArr.size() - 1].HS = m_hullShader;

		IndexShader = (int)HullShderArr.size();

	}

	if (CSOFileName[Len - 5] == 's' && CSOFileName[Len - 6] == 'd')
	{

		ID3D11DomainShader * m_domainShader;

		hr = active_D3DGC->D11_device->CreateDomainShader ( Blob->GetBufferPointer (), Blob->GetBufferSize (), NULL, &m_domainShader );
		if (FAILED ( hr ))
		{
			MessageBox ( active_hwnd, L"ResourceManager. ������ � �������� ������ �������.", Error, MB_OK );
			return hr;
		}

		DomainShderArr.push_back ( m_domainShader );
		BunchArr[BunchArr.size () - 1].DS = m_domainShader;

		IndexShader = (int)DomainShderArr.size ();

	}

	// ���� ������ � ������ �� �����������!
	if (CSOFileName[Len - 5] == 's' && CSOFileName[Len - 6] == 'c') 
	{
		ID3D11ComputeShader * m_ComShader;

		hr = active_D3DGC->D11_device->CreateComputeShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &m_ComShader);
		if ( FAILED( hr ) )
		{
			MessageBox(active_hwnd, L"ResourceManager. ������ � �������� ���������� ������� �������.", Error, MB_OK);
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

	ID3D11ShaderResourceView * ShaderResture;
	ID3D11Resource * LoadedTextureResource;

	length = (UINT)wcslen(TextureFilename);

	if ((TextureFilename[length - 3] == 'p' && TextureFilename[length - 2] == 'n' && TextureFilename[length - 1] == 'g') ||
		(TextureFilename[length - 3] == 'j' && TextureFilename[length - 2] == 'p' && TextureFilename[length - 1] == 'g') ||
		(TextureFilename[length - 3] == 'b' && TextureFilename[length - 2] == 'm' && TextureFilename[length - 1] == 'p')) 
	{


		hr = DirectX::CreateWICTextureFromFile(active_D3DGC->D11_device, TextureFilename, &LoadedTextureResource, &ShaderResture, NULL);
		if ( FAILED( hr ) ) {
			MessageBox(active_hwnd, L"���� ���������.ResourceManager", Error, MB_OK);
			return hr;
		}

		ShaderResourceArr.push_back(ShaderResture);
		ResourceArr.push_back(LoadedTextureResource);

		Result = (int)(ShaderResourceArr.size() - 1);
		TexturesNum++;

	}
    if (TextureFilename[length - 3] == 'd' && TextureFilename[length - 2] == 'd' && TextureFilename[length - 1] == 's') 
	{


		hr = DirectX::CreateDDSTextureFromFile(active_D3DGC->D11_device, TextureFilename, &LoadedTextureResource, &ShaderResture, NULL);
		if ( FAILED( hr ) ) {
			MessageBox(active_hwnd, L"���� ���������.ResourceManager", Error, MB_OK);
			return hr;
		}

		ShaderResourceArr.push_back(ShaderResture);
		ResourceArr.push_back(LoadedTextureResource);

		Result = (int)(ShaderResourceArr.size() - 1);
		TexturesNum++;
	}

	return Result;

}

void ResourceManager::decodeFile(unsigned char ** DestBuff , WCHAR * kafFilename , HWND hwnd) {

	std::basic_ifstream<unsigned char> Readfile(kafFilename, std::ios::in | std::ios::binary);


	if (!Readfile) {
		MessageBox(hwnd, L"�� ������� ����� �� ��������� ������.ResourceManager", Error, MB_OK);
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
	decodeFile(&Buff, kafFilename, active_hwnd);
	unsigned char IsKaf[4];
	int DescStart, objNum;
	unsigned char ObjFormat[3];
	int ReadingFileSize = 0;

	temp = Buff;
	memcpy(IsKaf, temp, 4);
	temp += 4;

	if (IsKaf[0] != ' ' && IsKaf[1] != 'K' && IsKaf[2] != 'A' && IsKaf[3] != 'F') 
	{
		MessageBox(active_hwnd, L"������� ���� �� �������� kaf ������.ResourceManager", Error, MB_OK);
		return E_FAIL;
	}


	memcpy((unsigned char*)&DescStart, temp, sizeof(int)); ///  ����� ��� ���������� ��������
	temp += sizeof(int);

	DestPointer = Buff + DescStart; // ������� ��������� �� ������ ������� ������� �������� ����� ������ ��� ������

	memcpy((unsigned char*)&objNum, temp, sizeof(int)); ///  ����� ���������� ��������
	temp += sizeof(int);

	int c = 0;
	std::wstring * Names; //  ������ ���� ��������

	// ����� ����� �������************************************************************
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
	// ����� ����� �������************************************************************

	c = 0;
	int length;

	IDirectSound3DBuffer * temp3DBuffer = nullptr;
	IDirectSoundBuffer8 * tempSoundbuff = nullptr;
	unsigned char * TempFile;
	// ���� ��������������� ���������� 3d � �������� �������� ������*********************************************
	while (c < objNum) {

		memcpy((unsigned char*)&ObjFormat, temp, sizeof(unsigned char) * 3); ///  ����������� �����
		temp += sizeof(unsigned char) * 3;

		memcpy((unsigned char*)&ReadingFileSize, temp, sizeof(int)); ///  ����� ������ �����
		temp += sizeof(int);

		TempFile = new unsigned char[ReadingFileSize ];


		memcpy(&TempFile[0], temp, ReadingFileSize); ///  ����� ����
		temp += ReadingFileSize;


		length = (int)Names[c].length();
		// ���� ���� wav
		if (Names[c][length - 3] == 'w' && Names[c][length - 2] == 'a' && Names[c][length - 1] == 'v') {

			// ���� ���� ������ ���� 3D
			if (Names[c][length - 7] == '_' && Names[c][length - 6] == '3' && Names[c][length - 5] == 'd') {

				ActiveSound->LoadMonoWaveFile(TempFile , &tempSoundbuff , &temp3DBuffer);

				ActiveSound->_3DSound.push_back(tempSoundbuff);
				ActiveSound->_3DSoundControl.push_back(temp3DBuffer);
			}

			// ���� ���� ������ ���� �������
			else{

				ActiveSound->LoadStereoWaveFile(TempFile, &tempSoundbuff);
				ActiveSound->SoundStereo.push_back(tempSoundbuff);

			}

		}

		// ���� ���� ogg
		if (Names[c][length - 3] == 'o' && Names[c][length - 2] == 'g' && Names[c][length - 1] == 'g') {

			// ���� ���� ������ ���� 3D
			if (Names[c][length - 7] == '_' && Names[c][length - 6] == '3' && Names[c][length - 5] == 'd') {

				// ��� ���� �� �������� ������

			}

			// ���� ���� ������ ���� �������
			else {

				ActiveSound->LoadOggMusicFile(TempFile, &tempSoundbuff, ReadingFileSize);

				ActiveSound->SoundStereo.push_back(tempSoundbuff);

			}


		}

		delete[] TempFile;

		++c;
	}
	// ���� ��������������� ���������� 3d � �������� �������� ������*********************************************

	delete[] Names;
	delete[] Buff;

	return S_OK;
}

void  ResourceManager::Init(D3DGlobalContext * g_D3DGC, HWND g_hwnd) 
{

	active_D3DGC = g_D3DGC;
	active_hwnd = g_hwnd;
}

void ResourceManager::SetActiveComeputeShader(int ShaderIndex) 
{

	active_D3DGC->D11_deviceContext->CSSetShader(ComeputeShaderArr[ShaderIndex] , NULL, 0);

}

void ResourceManager::SetNullAllShaders() {

	active_D3DGC->D11_deviceContext->PSSetShader(NULL, NULL, 0);

	active_D3DGC->D11_deviceContext->VSSetShader(NULL, NULL, 0);

	active_D3DGC->D11_deviceContext->GSSetShader(NULL, NULL, 0);

	active_D3DGC->D11_deviceContext->HSSetShader(NULL, NULL, 0);
    
	active_D3DGC->D11_deviceContext->CSSetShader(NULL, NULL, 0);
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
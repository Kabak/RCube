#include "stdafx.h"
#include "StringsListClass.h"
#include "ResourceManager.h"

StringsListClass::StringsListClass()
{
				 Enabled = true;
				 Scaling = 1.0f;
			Scroll_Y_Pos = 0;
	 First_Visible_Index = 0;
	  Last_Visible_Index = 0;
	  
	  StringListObj = nullptr;
}


StringsListClass::~StringsListClass()
{
	ResManager->Delete_Flat_ObjectBuffers ( StringListObjBufferIndex );
	RCUBE_DELETE ( StringListObj );
}


void StringsListClass::ShutDown()
{

	int Size = (int)SentencesResIndex.size();
	for (int i = 0; i < Size ; ++i)
	{
		ResManager->DeleteSentence( SentencesResIndex[i] );
	}

	SentencesResIndex.clear();
}


HRESULT StringsListClass::Init( 
	D3DGlobalContext* D3DGC,
	XMFLOAT4& _FormCoord,
	StringsList_Elements& StringsListClassInit,
	ResourceManager * _GlobalResourceManager
	)
{
	HRESULT Result = S_OK;

	 Local_D3DGC  = D3DGC;
	 Default_RTV  = D3DGC->BackBuffer_RTV;
	 StrList_RTV  = D3DGC->BackBuffer_ProxyTextureRTV;
	 StrList_SRV  = D3DGC->BackBuffer_ProxyTextureSRV;

	  ResManager = _GlobalResourceManager;

	  Strings = StringsListClassInit.Strings;

	    ObjParam = StringsListClassInit.ObjParam;
ObjOriginalParam = ObjParam;

	  ObjParam_X = ( int ) ObjParam.x;
	  ObjParam_Y = ( int ) ObjParam.y;
	  ObjParam_Z = ( int ) ObjParam.z;
	  ObjParam_W = ( int ) ObjParam.w;

  SentencesIndex = StringsListClassInit.SentencesIndex;
	   FontIndex = StringsListClassInit.FontIndex;

 ShowStringsList = StringsListClassInit._ShowStringsList;
		 Enabled = StringsListClassInit.Enabled;
StringsMAXLength = StringsListClassInit.StringsMAXLength;
  StringsSpacing = StringsListClassInit.StringsSpacing;
		 Scaling = StringsListClassInit.Scaling;
	 ScrollSpeed = StringsListClassInit.ScrollSpeed;

	   FormCoord = _FormCoord;

First_Visible_Index = 0;

StringListObj = new FlatObjectClass;

StringListObjBufferIndex = ResManager->Create_Flat_Obj_Buffers ( NO_CPU_ACCESS_BUFFER, 4, 6, nullptr );

StringListObj->Init ( D3DGC->ScreenWidth, D3DGC->ScreenHeight, ObjParam, D3DGC->BackBuffer_ProxyTextureSRV, NO_FLIP, ResManager->Get_Flat_ObjectBuffers_ByIndex ( StringListObjBufferIndex ) );

UpdateBodyPos ();

FontHeightInPixel = ResManager->GetFontHeightInPixels( FontIndex );

if ( FontHeightInPixel == -1 )
{
	MessageBox( NULL, L"InitStringsListClass Font Index Error ", L"Error", 0 );
	return false;
}

Result = SetInitFrameData();
	if ( FAILED( Result ) )
		MessageBox( NULL, L"StringsList SetInitFrameData Error", 0, 0 );

	// ���������� � �������� �� 0 �� 1
	float X_OnePixel = 1 / ( float ) Local_D3DGC->ScreenWidth;
	float Y_OnePixel = 1 / ( float ) Local_D3DGC->ScreenHeight;

	// ���������� ���� � �������� Local_D3DGC->BackBuffer_ProxyTextureRTV � ������� �� ������ ������
	TextureLeftTop		= { ObjParam.x * X_OnePixel, ObjParam.y * Y_OnePixel , 0.0f , 0.0f };
	TextureRightBottom	= {(ObjParam.x + ObjParam.z) * X_OnePixel, (ObjParam.y + ObjParam.w) * Y_OnePixel , 0.0f , 0.0f };
	TextureLeftBottom	= { ObjParam.x * X_OnePixel, ( ObjParam.y + ObjParam.w ) * Y_OnePixel , 0.0f , 0.0f };
	TextureRightTop		= {(ObjParam.x + ObjParam.z) * X_OnePixel, ObjParam.y * Y_OnePixel , 0.0f , 0.0f };

	Vertex_FlatObject* vertices = new Vertex_FlatObject[4];

	vertices[0].Position = XMFLOAT3( StringListObj->left, StringListObj->top, 0.0f );  // Top left.
	vertices[0].TexCoord = TextureLeftTop;

	vertices[1].Position = XMFLOAT3( StringListObj->right, StringListObj->bottom, 0.0f );  // Bottom right.
	vertices[1].TexCoord = TextureRightBottom;

	vertices[2].Position = XMFLOAT3( StringListObj->left, StringListObj->bottom, 0.0f );  // Bottom left.
	vertices[2].TexCoord = TextureLeftBottom;

	vertices[3].Position = XMFLOAT3( StringListObj->right, StringListObj->top, 0.0f );  // Top right.
	vertices[3].TexCoord = TextureRightTop;

	StringListObj->Buffers->FlatObjectVB->Update ( vertices );
	StringListObj->Buffers->IndexBs->Update ( (Index_Type*)FlatObjectIndices );

	delete[] vertices;

	UpdateABSElementAll();

return Result;
}


bool StringsListClass::SetInitFrameData()
{

	StringsNumber = ( int ) Strings.size();	// �������� ����� ���������� �����

	// ���������� ������� ����� � ��������� ����� ������� ����������� � ���� ��� ���������
	MaxVisibleIndex = ObjParam_W / int(float( FontHeightInPixel + StringsSpacing ) * Scaling);

	++MaxVisibleIndex; // ����������� ������ ��� ��������� ��������� � ���������

	// ���� ����� ������ ��� ������� ������, �� ������������� ���������� ������������ ����� 
	// ������ ���������� ������������ � ������ ��������� ( � ������ ����������� ����� �������� ) 
	if ( StringsNumber > MaxVisibleIndex )
		Last_Visible_Index = MaxVisibleIndex;
	else
		Last_Visible_Index = StringsNumber - 1;

	// ������ ��������� �������� ����� 
	SentencesResIndex.reserve( MaxVisibleIndex ); // +1 ��� ��������� ������ ��� ��������� StringsListClass

	int HaveIndex;
	SENTENCE_INIT_DATA Data;
	ZeroMemory( &Data, sizeof( SENTENCE_INIT_DATA ) );

		Data.Colour = { 1.0f, 1.0f, 1.0f, 0.0f };	//	w = 0.0f  For correct render StringList Sentences
		Data.FontType   = FontIndex;
		Data.HideType   = HIDE;
		Data.ShowType   = SHOW;
		Data.Level	    = SentencesIndex;
		Data.MaxLength  = StringsMAXLength;
		Data.Render	    = true;
		Data.PosX		= ObjParam_X;
		Data.PosY		= 0;

		int OffsetY		= ObjParam_Y; // ��� ������������ ����� ������� �����
	// ����������� ������ � ������ Sentence
	for (int i = 0 ; i < Last_Visible_Index; ++i )
	{
		Data.PosY += OffsetY;
		// ����������� � ������ ���������� ����������� ������������ � ������ StringsList + ���� ��������� ��� ���������
		// ���� ����������� ������ ��� ���������� � ������ ���������.
		HaveIndex = ResManager->AddSentence( &Data, Strings[i] );
		if ( HaveIndex != -1 )
			SentencesResIndex.emplace_back( HaveIndex );
		else
		{
			MessageBox(NULL, L"Can't create StringsListClass Sentance", L"Error", 0);
			return false;
		}
		OffsetY = StringsHeigh = (int)((float)StringsSpacing + (float)FontHeightInPixel * Scaling );
	}

	return true;
}


// ScrollDir = 1 - Up, = 0 - Down
void StringsListClass::ScrollStringsList( bool ScrollDir )
{
	if ( ScrollDir )
	{
		// �������� ����� ������ ����� 
		// ���� ��������� ������ �����, ��
		if ( Last_Visible_Index == StringsNumber - 1 )
		{
			// ��������� ������ ����� ��������� ?  ���� ���, �� ������� ��� ������� ������ �����.
			int a = ResManager->GetSentencePosY( SentencesResIndex[MaxVisibleIndex - 1] );
			if ( a + StringsHeigh >= ObjParam_Y + ObjParam_W )
				ScrollUp();
		}
		// ����� �� ��������� ������ � ������. 
		else
		{
			// ����� �������� ������ ������� � StringsList �����
			UpdateVisibleSentence( 1 );
			// �������� ������ �����
			ScrollUp();
		}
	}
	else
	{
		// ���� ������ StringsList ����� ����� ������� ����������� �� ������, ��
		if ( First_Visible_Index == 0 )
		{
			// ���� ������� 1-�� ����������� ���� ���� ���������, �� ����� �������� ��� ������ ����
			if (ResManager->GetSentencePosY( SentencesResIndex[0] ) < ObjParam_Y )
				ScrollDown();
		}
		// ���� ������ StringsList ����� �� ������ ����������� �� ������ ������, �� ����� �������� ����
		else if( First_Visible_Index > 0 )
		{
			UpdateVisibleSentence( 0 );
			ScrollDown();
		}

	}
}


// �������� ������� ������ � StringsList | ScrollDir = 1 - Up, = 0 - Down
void StringsListClass::UpdateVisibleSentence( bool ScrollDir )
{
	// ������� �����
	if ( ScrollDir )
	{
		int a = ResManager->GetSentencePosY( SentencesResIndex[0] ) + StringsHeigh;
		// ������ ����������� �������� �� ���� ������
		if ( a <= ObjParam_Y )
		{
			int TempIndex = SentencesResIndex[0];
			
			SentencesResIndex.erase( SentencesResIndex.begin() );

			int LastIndex = (int)SentencesResIndex.size();

			ResManager->UpdateSentence(
			TempIndex, 
			Strings[ Last_Visible_Index + 1 ],
			ObjParam_X,
			ResManager->GetSentencePosY( SentencesResIndex[LastIndex - 1] ) + StringsHeigh
			);

			SentencesResIndex.emplace_back( TempIndex );

			++Last_Visible_Index;
			++First_Visible_Index;

		}
	}
	// ������� ����
	else
	{
		int a = ResManager->GetSentencePosY( SentencesResIndex[MaxVisibleIndex - 1] );
		// ������ ����������� �������� �� ���� ������
		if ( a >= ObjParam_Y + ObjParam_W )
		{
			int TempIndex = SentencesResIndex[MaxVisibleIndex - 1];

			SentencesResIndex.pop_back();

			int LastIndex = ( int ) SentencesResIndex.size();

			ResManager->UpdateSentence(
				TempIndex,
				Strings[First_Visible_Index - 1],
				ObjParam_X,
				ResManager->GetSentencePosY( SentencesResIndex[0] ) - StringsHeigh
				);

			SentencesResIndex.emplace( SentencesResIndex.begin(), TempIndex );

			--Last_Visible_Index;
			--First_Visible_Index;
		}
	}
}


void StringsListClass::ScrollUp()
{
	Scroll_Y_Pos -= ScrollSpeed;
	UpdateVisibleSentance();
	Scroll_Y_Pos = 0;
}


void StringsListClass::ScrollDown()
{
	Scroll_Y_Pos += ScrollSpeed;
	UpdateVisibleSentance();
	Scroll_Y_Pos = 0;
}


void StringsListClass::UpdateVisibleSentance()
{
	int Size = (int)SentencesResIndex.size();
	int i = 0;

	do
	{
		ResManager->UpdateSentence(
		SentencesResIndex[i],
			ResManager->GetSentenceText( SentencesResIndex[i] ),
		ObjParam_X,
		ResManager->GetSentencePosY( SentencesResIndex[i] ) + Scroll_Y_Pos
		);
	++i;
	}
	while ( i < Size );
}


int StringsListClass::Frame( DXINPUTSTRUCT& InputClass, FPSTimers& fpstimers, bool &ObjectBUSY )
{
	if ( ObjectBUSY && !StringsListPressed )
		return 0;

	if ( Enabled )
	{
	// ���� ������ ����� ������ ���� � ���������� ����� ���� ����� ��� ����
	if ( ( InputClass.MousePos.x > ABSoluteX && InputClass.MousePos.x < ABSoluteX + ABSolute_Width ) &&
		 ( InputClass.MousePos.y > ABSoluteY && InputClass.MousePos.y < ABSoluteY + ABSolute_Height ) )
	{
		if ( InputClass.m_mouseState.rgbButtons[0] )
		{
			if ( InputClass.m_mouseDY > 0 )
			{
				ScrollStringsList( 0 );
			}
			else if ( InputClass.m_mouseDY < 0 )
			{
				ScrollStringsList( 1 );
			}


		}
		// ���� ������� ��������� ������
		else if ( InputClass.m_mouseDZ != 0 )
		{
			if ( InputClass.m_mouseDZ > 0 )
			{
				ScrollStringsList( 0 );
			}
			else if ( InputClass.m_mouseDZ < 0 )
			{
				ScrollStringsList( 1 );
			}
		}
	} // ����� ������ �������

	} // Enabled

	return 0;
}


void StringsListClass::UpdateABSElementAll()
{
	ABSoluteX = long( FormCoord.x + ObjParam.x );
	ABSoluteY = long( FormCoord.y + ObjParam.y );
	ABSolute_Width = ( long ) ObjParam.z;
	ABSolute_Height = ( long ) ObjParam.w;
}


void StringsListClass::UpdateBodyPos ()
{
	StringListObj->left = - StringListObj->_2DPixelXmax + ( FormCoord.x + ObjParam.x );
	StringListObj->right = StringListObj->left + ObjParam.z;
	StringListObj->top = StringListObj->_2DPixelYmax - ( FormCoord.y + ObjParam.y );
	StringListObj->bottom = StringListObj->top - ObjParam.w;
}

/*
bool StringsListClass::SaveTextureToPNG( ID3D11ShaderResourceView* Texture )
{
	bool Result = true;
	HRESULT hr;

	ID3D11Resource* tempResource = nullptr;
	ID3D11Texture2D* InputTexture = nullptr;
	D3D11_TEXTURE2D_DESC* InputTextureDesc = new D3D11_TEXTURE2D_DESC;
	D3D11_TEXTURE2D_DESC* desc = new D3D11_TEXTURE2D_DESC;

	Texture->GetResource( &tempResource );
	tempResource->QueryInterface<ID3D11Texture2D>( &InputTexture );
	InputTexture->GetDesc( InputTextureDesc );

	ZeroMemory( desc, sizeof( D3D11_TEXTURE2D_DESC ) );
	desc->BindFlags = 0;//D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc->Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc->Height = InputTextureDesc->Height;
	desc->Width = InputTextureDesc->Width;
	desc->ArraySize = 1;
	desc->SampleDesc.Count = 1;
	desc->SampleDesc.Quality = 0;
	desc->MipLevels = 1;
	desc->CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	desc->Usage = D3D11_USAGE_STAGING;
	// ���������� �������� ������ ��� �������� ��� MSAA 
	// ����� ������ ResolveSubresource

	hr = Local_D3DGC->DX_device->CreateTexture2D( desc, 0, &Local_D3DGC->ScreenShootTexture );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Can't create ScreenShotTexture", 0, 0 );
		Result = false;
		goto END;
	}

	// ���� �������� MSAA, �� ������ ResolveSubresurces
	if ( InputTextureDesc->SampleDesc.Count > 1 )
	{
		Local_D3DGC->DX_deviceContext->ResolveSubresource( Local_D3DGC->BackBuffer_CopyResolveTexture, 0, tempResource, 0, DXGI_FORMAT_R8G8B8A8_UNORM );
		// D3DGC->BackBuffer_CopyResolveTextureSRV
		ID3D11Resource* Resource;
		Local_D3DGC->BackBuffer_CopyResolveTextureSRV->GetResource( &Resource );
		Local_D3DGC->DX_deviceContext->CopyResource( Local_D3DGC->ScreenShootTexture, Resource );
		Resource->Release();
	}
	else
	{
		Local_D3DGC->DX_deviceContext->CopyResource( Local_D3DGC->ScreenShootTexture, tempResource );
	}

	union _block
	{
		byte  *data;
		unsigned int *udata;
	} block;

	D3D11_MAPPED_SUBRESOURCE  mapResource;
	hr = Local_D3DGC->DX_deviceContext->Map( Local_D3DGC->ScreenShootTexture, 0, D3D11_MAP_READ, NULL, &mapResource );

	unsigned int amount = mapResource.DepthPitch; // RowPitch * Height;
	block.data = new byte[amount];
	memcpy( block.data, mapResource.pData, amount );
	Local_D3DGC->DX_deviceContext->Unmap( Local_D3DGC->ScreenShootTexture, 0 );

	Image img;
	img.height = desc->Height;
	img.width = desc->Width;
	img.format = desc->Format;
	img.rowPitch = mapResource.RowPitch;
	img.slicePitch = mapResource.DepthPitch;
	img.pixels = block.data;

	hr = SaveToWICFile( img, WIC_FLAGS_NONE, GetWICCodec( WIC_CODEC_PNG ), L"Texture.png" );
	if ( FAILED( hr ) )
	{
		assert( L"Could not save Texture.png file." );
		Result = false;
	}

	delete[] block.data;

END:

	delete InputTextureDesc;
	delete desc;

	RCUBE_RELEASE( tempResource );
	RCUBE_RELEASE( InputTexture );
	RCUBE_RELEASE( Local_D3DGC->ScreenShootTexture );

	return Result;
}
*/

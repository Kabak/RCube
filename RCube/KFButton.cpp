#include "stdafx.h"
#include "KFButton.h"

KFButton::KFButton()
{
//	memset(this, 0, sizeof(KFButton));

			 IsButonPressTexture = nullptr;
		  IsMouseOnButtonTexture = nullptr;
			  NotEnalbledTexture = nullptr;
							 Obj = nullptr;
				 g_State.IsPress = false ;
		 g_State.IsMouseOnButton = false ;
						 Checked = false;
					  ObjectType = 0;
					  EditType	 = 0;
			 NeedReleaseOnObject = -1;
						 Enabled = true;// �������� �� ���������
				   SentenceIndex = -1;	// ��� ������ ����������� � ����������
				    ActiveAsEdit = false;
						CursorOn = false;
}

HRESULT KFButton::Init(
	D3DGlobalContext* D3DGC,
			XMFLOAT4& _ScreenCoords,
			XMFLOAT4& _FormCoord,
   KFButton_Elements& ButtonInitData
		  ) 
	{
	HRESULT Result = S_OK ;

	 D3DGC_Obj = D3DGC;

	Checked    = ButtonInitData.Checked;
	ObjectType = ButtonInitData.Type;
	EditType   = ButtonInitData.EditType;
	Enabled    = ButtonInitData.Enabled;

	     SentenceIndex = ButtonInitData.SentenceIndex;
	WaitingForKeyPress = ButtonInitData.WaitingForKeyPress;
	   VirtualKeyIndex = ButtonInitData.VirtualKeyIndex;
			SecondSlot = ButtonInitData.SecondSlot;

		  ScreenCoords = _ScreenCoords;
			 FormCoord = _FormCoord;

			  ObjParam = ButtonInitData._ObjParam;
	
	  OsnButtontexture = ButtonInitData.OsnTexture;
	
	// ����������� �������� ��������
	if ( ButtonInitData.OsnTextureResource )
	{
		D3D11_TEXTURE2D_DESC* InputTextureDesc = new D3D11_TEXTURE2D_DESC;
		ID3D11Texture2D* InputTexture;
		ButtonInitData.OsnTextureResource->QueryInterface<ID3D11Texture2D>(&InputTexture);
		InputTexture->GetDesc(InputTextureDesc);

		if ( ObjParam.z == 0.0f )
			ObjParam.z = (float)InputTextureDesc->Width;

		if ( ObjParam.w == 0.0f )
			ObjParam.w = (float)InputTextureDesc->Height;

		delete InputTextureDesc;
		InputTexture->Release();
	}
	else
	{
		ObjParam.z = 0.0f;
		ObjParam.w = 0.0f;
	}

	ObjOriginalParam = ObjParam;

	_2DPixelXmax = float(ScreenCoords.z / 2);
	_2DPixelYmax = float(ScreenCoords.w / 2);

	UpdateABSElementAll();

	SetButtonParam();

	Vertex_FlatObject* vertices ;
	// Create the vertex array.
	vertices = new Vertex_FlatObject[4];
	if(!vertices)
	{
		return E_FAIL ;
	}

	vertices[0].Position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].TexCoord = XMFLOAT2(0.0f, 0.0f);

	vertices[1].Position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].TexCoord = XMFLOAT2(1.0f, 1.0f);

	vertices[2].Position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].TexCoord = XMFLOAT2(0.0f, 1.0f);

	vertices[3].Position = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[3].TexCoord = XMFLOAT2(1.0f, 0.0f);


	unsigned long indices[6] = { 0, 1, 2, 0, 3, 1 };

	Obj = new KF2DObjClass ;
	Obj->Init( D3DGC, vertices, indices, ButtonInitData.OsnTexture, 4, 6 );

	delete [] vertices ;
	
	return Result ;
}

HRESULT KFButton::Draw()
{

	Obj->Render() ;
	
	return S_OK;
}

void KFButton::SetIfButtonPressTexture(ID3D11ShaderResourceView* Texture){

	IsButonPressTexture = Texture;
	
}

void KFButton::SetIfButtonNotEnalbledTexture(ID3D11ShaderResourceView* Texture){

	NotEnalbledTexture = Texture;

}

void KFButton::SetIfMouseOnButtonTexture(ID3D11ShaderResourceView* Texture){

	IsMouseOnButtonTexture = Texture;

}

void KFButton::GetButtonState(ButtonState & State) {

	State = g_State ;

}

void KFButton::ClearButtonState()
{
	g_State.IsPress = false;
}


KFButton::~KFButton()
{

	RCUBE_DELETE( Obj );
}


void KFButton::SetOsnTexture(ID3D11ShaderResourceView* Texture){

	OsnButtontexture = Texture ;

}


void KFButton::ResetButtonParam() {

	UpdateABSElementAll();

	SetButtonParam();

	Vertex_FlatObject* vertices ;
	// Create the vertex array.
	vertices = new Vertex_FlatObject[4];


	vertices[0].Position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].TexCoord = XMFLOAT2(0.0f, 0.0f);

	vertices[1].Position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].TexCoord = XMFLOAT2(1.0f, 1.0f);

	vertices[2].Position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].TexCoord = XMFLOAT2(0.0f, 1.0f);

	vertices[3].Position = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[3].TexCoord = XMFLOAT2(1.0f, 0.0f);

	Obj->SetVertexBuffer( vertices ) ;

	delete [] vertices ;

}

bool KFButton::Frame( DXINPUTSTRUCT& InputClass, bool &ObjectBUSY )
{
	// ��������, ���� �� ������ ����� � �� �������� ������ �� ���� �������
	// ���� �� ���� ������ ����� ��� ������ �� ���� �������, �� ��������� ��������� ����� �������
	if ( ObjectBUSY && NeedReleaseOnObject == -1 )
		return false;

	bool ActiveEditChanged = false;

	g_State.IsPress = false;
	// ���� ������ �������
	if (Enabled == true) 	
	{

		g_State.IsMouseOnButton = false;
		// ���� ������ CHeckBox � ����������
		if ( ObjectType == 1 && Checked )
			g_State.IsPress = true;
		else
			g_State.IsPress = false;

		// �p������� ������ �� ����� � ������
		if ((( InputClass.MousePos.x > ABSoluteX && InputClass.MousePos.x < ABSoluteX + ABSolute_Width ) &&
			 ( InputClass.MousePos.y > ABSoluteY && InputClass.MousePos.y < ABSoluteY + ABSolute_Height )))
		{
			if ( InputClass.m_mouseState.rgbButtons[0] )  // ����� ���� ������ �� �������
			{
				
				switch ( ObjectType )
				{
					case 0:
								NeedReleaseOnObject = 0;
								ObjectBUSY = true;
								break;

					// CheckBox ���� ������ � ���������
					case 1: 
								NeedReleaseOnObject = 1;
								ObjectBUSY = true;
								break;
					// Label
					case 2:
								NeedReleaseOnObject = 2;
								ObjectBUSY = true;
								break;
					// Edit
					case 3:
								NeedReleaseOnObject = 3;
								ObjectBUSY = true;
								break;
					default:;				
				}

				if ( IsButonPressTexture != NULL || Checked )
				{
					SetObjectTexture( IsButonPressTexture );
				}
				else
					SetObjectTexture( OsnButtontexture );
			}
			else // ����� �� ���� ������ �� �������
			{ 
				switch ( ObjectType )
				{
					// Button
					case 0:
						if (NeedReleaseOnObject == 0)
						{
							NeedReleaseOnObject = -1;
							ObjectBUSY = false;
							g_State.IsPress = true; // ���� ��� ��������					
						}
						break;

					// CheckBox
					case 1:
						if ( NeedReleaseOnObject == 1 )
						{
							if (Checked)
							{
								Checked = false;
								NeedReleaseOnObject = -1;
								ObjectBUSY = false;
								g_State.IsPress = true;
							}
							else
							{
								Checked = true;
								NeedReleaseOnObject = -1;
								ObjectBUSY = false;
								g_State.IsPress = true;
							}
						}
						break;
					// Label
					case 2:
						if (NeedReleaseOnObject == 2)
						{
							NeedReleaseOnObject = -1;
							ObjectBUSY = false;
							if ( WaitingForKeyPress )
							{
								ActiveAsEdit = true;
								ActiveEditChanged = true;
							}
						}
						break;
					// Edit
					case 3:
						if (NeedReleaseOnObject == 3)
						{
							NeedReleaseOnObject = -1;
							ObjectBUSY = false;
							ActiveAsEdit = true;
							ActiveEditChanged = true;
						}

				default:;
				}

				if (IsMouseOnButtonTexture != NULL)
				{
					SetObjectTexture(IsMouseOnButtonTexture);
				}
				else 
				{
					if ( Checked )
						SetObjectTexture( IsButonPressTexture );
					else
						SetObjectTexture( OsnButtontexture );
				}
			}	

			g_State.IsMouseOnButton = true;

		}
		else
		{
			// ���������� ������� �� ������, ���� ������� ���� ��������� ������� ����� � �������
			NeedReleaseOnObject = -1;
			ObjectBUSY = false;

			if ( Checked )
			{
				SetObjectTexture(IsButonPressTexture);
//				g_State.IsPress = true;
			}
			else
			{
				SetObjectTexture(OsnButtontexture);
//				g_State.IsPress = false;
			}

		}
	} // ���� ������ �� �������
/*	else if (NotEnalbledTexture != nullptr) 
	{
		SetObjectTexture( NotEnalbledTexture );
	}
	else
	{
		SetObjectTexture( OsnButtontexture );
	}
*/
	return ActiveEditChanged;
}


void KFButton::SetButtonParam()
{
	left = -_2DPixelXmax + (FormCoord.x + ObjParam.x);
	right = left + ObjParam.z;
	top = _2DPixelYmax - (FormCoord.y + ObjParam.y);
	bottom = top - ObjParam.w;
}


void KFButton::UpdateABSElementSize()
{
	ABSolute_Width = (long)ObjParam.z;
	ABSolute_Height = (long)ObjParam.w;
}


void KFButton::UpdateABSElementAll()
{
	ABSoluteX = long(FormCoord.x + ObjParam.x);
	ABSoluteY = long(FormCoord.y + ObjParam.y);
	ABSolute_Width = (long)ObjParam.z;
	ABSolute_Height = (long)ObjParam.w;
}


void KFButton::UpdateABSElementPos()
{
	ABSoluteX = long(FormCoord.x + ObjParam.x);
	ABSoluteY = long(FormCoord.y + ObjParam.y);
}


void KFButton::SetEnable ( bool Value )
{
	if( Value )
	{
		Enabled = true;
		SetObjectTexture( OsnButtontexture );
	}
	else
	{
		Enabled = false;
		if( NotEnalbledTexture != nullptr )
		{
			SetObjectTexture( NotEnalbledTexture );
		}
		else
		{
			SetObjectTexture( OsnButtontexture );
		}
	}

}
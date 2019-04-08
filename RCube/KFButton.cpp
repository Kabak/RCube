#include "stdafx.h"
#include "KFButton.h"
#include "ResourceManager.h"

KFButton::KFButton()
{
//	memset(this, 0, sizeof(KFButton));

			 IsButonPressTexture = nullptr;
		  IsMouseOnButtonTexture = nullptr;
			  NotEnalbledTexture = nullptr;
			    OsnButtontexture = nullptr;
						 Buffers = nullptr;
						 Checked = false;
					  ObjectType = 0;
					  EditType	 = 0;
			 NeedReleaseOnObject = -1;
						 Enabled = true;// �������� �� ���������
				   SentenceIndex = -1;	// ��� ������ ����������� � ����������
				    ActiveAsEdit = false;
						CursorOn = false;
					   SetColour = false;
					EditFinished = false;
					     Changed = false;
}

HRESULT KFButton::Init(
	D3DGlobalContext* D3DGC,
			XMFLOAT4& _ScreenCoords,
			XMFLOAT4& _FormCoord,
   KFButton_Elements& ButtonInitData,
	Flat_Obj_Buffers* _Buffers,
	ResourceManager * _GlobalResourceManager
		  ) 
	{
	HRESULT Result = S_OK ;
	ResManager = _GlobalResourceManager;
	   Buffers = _Buffers;
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
	  if ( ObjectType == COLOR_PICK )
	  {
		  Colour = ButtonInitData.Colour;

		  ObjParam.z < 1.0f ? ObjParam.z = MIN_BUTTON_WIDTH : ObjParam.z;
		  ObjParam.w < 1.0f ? ObjParam.w = MIN_BUTTON_HEIGHT : ObjParam.w;
	  }
	  else if ( ButtonInitData.OsnTexture )
	  {
		  XMFLOAT4 Temp;
		  ResManager->GetTextureParam ( ButtonInitData.OsnTexture, Temp );

		  ObjParam.z < 1.0f ?  ObjParam.z = Temp.z : ObjParam.z;
		  ObjParam.w < 1.0f ?  ObjParam.w = Temp.w : ObjParam.w;
	  }
	  else
	  {
		  ObjParam.z = MIN_BUTTON_WIDTH;
		  ObjParam.w = MIN_BUTTON_HEIGHT;
	  }

	ObjOriginalParam = ObjParam;

	_2DPixelXmax = ScreenCoords.x / 2;
	_2DPixelYmax = ScreenCoords.y / 2;

	UpdateABSElementAll();

	UpdateBodyPos ();

	if ( ObjectType == COLOR_PICK )
	{
		SetButtonColor ( Colour );
	}
	else
	{
		Vertex_FlatObject* vertexes;
		vertexes = new Vertex_FlatObject[4];

		vertexes[0].Position = XMFLOAT3 ( left, top, 0.0f );  // Top left.
		vertexes[0].TexCoord = XMFLOAT4 ( 0.0f, 0.0f, 0.0f, 0.0f );

		vertexes[1].Position = XMFLOAT3 ( right, bottom, 0.0f );  // Bottom right.
		vertexes[1].TexCoord = XMFLOAT4 ( 1.0f, 1.0f, 0.0f, 0.0f );

		vertexes[2].Position = XMFLOAT3 ( left, bottom, 0.0f );  // Bottom left.
		vertexes[2].TexCoord = XMFLOAT4 ( 0.0f, 1.0f, 0.0f, 0.0f );

		vertexes[3].Position = XMFLOAT3 ( right, top, 0.0f );  // Top right.
		vertexes[3].TexCoord = XMFLOAT4 ( 1.0f, 0.0f, 0.0f, 0.0f );


		Buffers->FlatObjectVB->Update ( vertexes );
		Buffers->IndexBs->Update ( ( Index_Type* ) FlatObjectIndices );
		Buffers->RenderTexture = ButtonInitData.OsnTexture;

		delete [] vertexes;
	}
	return Result ;
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

}


void KFButton::SetOsnTexture(ID3D11ShaderResourceView* Texture){

	OsnButtontexture = Texture ;

}


void KFButton::ResetButtonParam() {

	UpdateABSElementAll();

	UpdateBodyPos ();

	if ( ObjectType == COLOR_PICK )
	{
		SetButtonColor ( Colour );
	}
	else
	{
		Vertex_FlatObject* vertexes;
		vertexes = new Vertex_FlatObject[4];

		vertexes[0].Position = XMFLOAT3 ( left, top, 0.0f );  // Top left.
		vertexes[0].TexCoord = XMFLOAT4 ( 0.0f, 0.0f, 0.0f, 0.0f );

		vertexes[1].Position = XMFLOAT3 ( right, bottom, 0.0f );  // Bottom right.
		vertexes[1].TexCoord = XMFLOAT4 ( 1.0f, 1.0f, 0.0f, 0.0f );

		vertexes[2].Position = XMFLOAT3 ( left, bottom, 0.0f );  // Bottom left.
		vertexes[2].TexCoord = XMFLOAT4 ( 0.0f, 1.0f, 0.0f, 0.0f );

		vertexes[3].Position = XMFLOAT3 ( right, top, 0.0f );  // Top right.
		vertexes[3].TexCoord = XMFLOAT4 ( 1.0f, 0.0f, 0.0f, 0.0f );
		
		Buffers->FlatObjectVB->Update ( vertexes );

		delete [] vertexes;
	}
}

bool KFButton::Frame( DXINPUTSTRUCT& InputClass, bool &ObjectBUSY)
{
	// ��������, ���� �� ������ ����� � �� �������� ������ �� ���� �������
	// ���� �� ���� ������ ����� ��� ������ �� ���� �������, �� ��������� ��������� ����� �������
	if ( ObjectBUSY && NeedReleaseOnObject == -1 )
		return false;

	bool ActiveEditChanged = false;
	
	   EditFinished = false;	// Reset Editing was finished last Frame
	        Changed = false;	// Reset Checkbox was changed last frame
	g_State.IsPress = false;
	// ���� ������ �������
	if (Enabled == true) 	
	{

		g_State.IsMouseOnButton = false;
		// ���� ������ CHeckBox � ����������
		if ( ObjectType == CHECKBOX && Checked )
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
					case BUTTON:
								NeedReleaseOnObject = BUTTON;
								ObjectBUSY = true;
								break;

					// CheckBox ���� ������ � ���������
					case CHECKBOX:
								NeedReleaseOnObject = CHECKBOX;
								ObjectBUSY = true;
								break;
					// Label
					case LABEL:
								NeedReleaseOnObject = LABEL;
								ObjectBUSY = true;
								break;
					// Edit
					case EDIT:
								NeedReleaseOnObject = EDIT;
								ObjectBUSY = true;
								break;
					case COLOR_PICK:
								NeedReleaseOnObject = COLOR_PICK;
								ObjectBUSY = true;
								break;
					default:;				
				}

				if ( IsButonPressTexture != NULL || Checked )
				{
					Buffers->RenderTexture = IsButonPressTexture;	// ����� ������� )
				}
				else
					Buffers->RenderTexture = OsnButtontexture;	// ����� ������� )
			}
			else // ����� �� ���� ������ �� �������
			{ 
				switch ( ObjectType )
				{
					// Button
					case BUTTON:
						if (NeedReleaseOnObject == BUTTON )
						{
							NeedReleaseOnObject = -1;
							ObjectBUSY = false;
							g_State.IsPress = true; // ���� ��� ��������					
						}
						break;

					// CheckBox
					case CHECKBOX:
						if ( NeedReleaseOnObject == CHECKBOX )
						{
							NeedReleaseOnObject = -1;

							if (Checked)
							{
								Checked = false;
								ObjectBUSY = false;
								g_State.IsPress = true;
								Changed = true;
							}
							else
							{
								Checked = true;
								ObjectBUSY = false;
								g_State.IsPress = true;
								Changed = true;
							}
						}
						break;
					// Label
					case LABEL:
						if (NeedReleaseOnObject == LABEL )
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
					case EDIT:
						if (NeedReleaseOnObject == EDIT )
						{
							NeedReleaseOnObject = -1;
							ObjectBUSY = false;
							ActiveAsEdit = true;
							ActiveEditChanged = true;
						}
					// COLOR
					case COLOR_PICK:
					if ( NeedReleaseOnObject == COLOR_PICK )
					{
						if ( SetColour )
						{
							SetColour = false;
							NeedReleaseOnObject = -1;
							ObjectBUSY = false;
							g_State.IsPress = true;
						}
						else
						{
							SetColour = true;
							NeedReleaseOnObject = -1;
							ObjectBUSY = false;
							g_State.IsPress = true;
						}
					}

				default:;
				}

				if (IsMouseOnButtonTexture != NULL)
				{
					Buffers->RenderTexture = IsMouseOnButtonTexture;
				}
				else 
				{
					if ( Checked )
						Buffers->RenderTexture = IsButonPressTexture;
					else
						Buffers->RenderTexture = OsnButtontexture;
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
				Buffers->RenderTexture = IsButonPressTexture;
			}
			else
			{
				Buffers->RenderTexture = OsnButtontexture;
			}

		}
	} // ���� ������ �� �������

	return ActiveEditChanged;
}


void KFButton::UpdateBodyPos ()
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
		Buffers->RenderTexture = OsnButtontexture;
	}
	else
	{
		Enabled = false;
		if( NotEnalbledTexture != nullptr )
		{
			Buffers->RenderTexture = NotEnalbledTexture;
		}
		else
		{
			Buffers->RenderTexture = OsnButtontexture;
		}
	}

}


void KFButton::SetButtonColor ( XMFLOAT4 _Color )
{
	if ( ObjectType == COLOR_PICK )
	{
		Vertex_FlatObject* vertexes;
		vertexes = new Vertex_FlatObject[4];

		Colour = _Color;

		GenerateVertexes ( vertexes, COLOR_BUTTON, Colour );

		Buffers->FlatObjectVB->Update ( vertexes );

		delete [] vertexes;
	}
}


void KFButton::SetButtonColorREF ( COLORREF Color )
{
	if ( ObjectType == COLOR_PICK )
	{
		Vertex_FlatObject* vertexes;
		vertexes = new Vertex_FlatObject[4];

		RCube_VecFloat234 RGBA;

		UINT R;
		UINT G;
		UINT B;
		UINT A;

		R = Color & 0x000000FF;
		G = ( Color & 0x0000FF00 ) >> 8;
		B = ( Color & 0x00FF0000 ) >> 16;
		A = ( Color & 0xFF000000 ) >> 24;

		RGBA.Fl4 = { float ( R ) , float ( G ) , float ( B ) , float ( A ) };

		RGBA.Vec /= 255.0f;

		Colour = RGBA.Fl4;

		GenerateVertexes ( vertexes, COLOR_BUTTON, RGBA.Fl4 );

		Buffers->FlatObjectVB->Update ( vertexes );

		delete [] vertexes;
	}
}


XMFLOAT4 KFButton::GetButtonColor ()
{
	return Colour;
}
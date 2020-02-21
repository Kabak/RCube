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
							Body = nullptr;
						 Checked = false;
					  ObjectType = 0;
					  EditType	 = 0;
			 NeedReleaseOnObject = -1;
						 Enabled = true;// Доступна по умолчанию
				   SentenceIndex = -1;	// Нет строки привязанной к компоненту
				    ActiveAsEdit = false;
						CursorOn = false;
					   SetColour = false;
					EditFinished = false;
					     Changed = false;
}


KFButton::~KFButton ()
{
	ResManager->Delete_Flat_ObjectBuffers ( BodyBufferIndex );
	RCUBE_DELETE ( Body );
}


HRESULT KFButton::Init(
	D3DGlobalContext* D3DGC,
			XMFLOAT4& _ScreenCoords,
			XMFLOAT4& _FormCoord,
   KFButton_Elements& ButtonInitData,
	ResourceManager * _GlobalResourceManager
		  ) 
	{
	HRESULT Result = S_OK ;
	ResManager = _GlobalResourceManager;
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
	  ObjOriginalParam = ObjParam;	// Save default demention & position  

	  OsnButtontexture = ButtonInitData.OsnTexture;
	
	// определение размеров текстуры
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

	UpdateABSElementAll();

	BodyBufferIndex = ResManager->Create_Flat_Obj_Buffers ( NO_CPU_ACCESS_BUFFER, 4, 6, ButtonInitData.OsnTexture );

	XMFLOAT4 BodyPos = { 0.0f, 0.0f, 0.0f, 0.0f };

	ConstructBody ( BodyPos );

	Body = new FlatObjectClass;
	Body->Init ( D3DGC->ScreenWidth, D3DGC->ScreenHeight, BodyPos, ButtonInitData.OsnTexture, NO_FLIP, ResManager->Get_Flat_ObjectBuffers_ByIndex ( BodyBufferIndex ) );

	Body->UpdateVertexPos ();

	if ( ObjectType == COLOR_PICK )
	{
		SetButtonColor ( Colour );
	}
	else
	{
		Vertex_FlatObject* BodyVertexes;
		BodyVertexes = new Vertex_FlatObject[4];
		
		Body->GenerateVertexes (BodyVertexes, NO_FLIP, Body->Dummy);

		Body->Buffers->FlatObjectVB->Update (BodyVertexes);

		delete [] BodyVertexes;
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


void KFButton::SetOsnTexture(ID3D11ShaderResourceView* Texture){

	OsnButtontexture = Texture ;

}


void KFButton::ResetButtonParam() {

	UpdateABSElementAll();

	Body->UpdateVertexPos ();

	if ( ObjectType == COLOR_PICK )
	{
		SetButtonColor ( Colour );
	}
	else
	{
		Vertex_FlatObject* BodyVertexes;
		BodyVertexes = new Vertex_FlatObject[4];

		Body->GenerateVertexes ( BodyVertexes, NO_FLIP, Body->Dummy );
	
		Body->Buffers->FlatObjectVB->Update ( BodyVertexes );

		delete [] BodyVertexes;
	}
}

bool KFButton::Frame( DXINPUTSTRUCT& InputClass, int &BUSY_Object_Index )
{
	// Проверка, была ли нажата мышка и не отпущена именно на этом объекте
	// Если не была нажата вовсе или нажата на этом объекте, то выполняем обработку этого объекта
	if ( BUSY_Object_Index != -1 && BUSY_Object_Index != ObjectIndex  )
		return false;

	bool ActiveEditChanged = false;
	
	   EditFinished = false;	// Reset Editing was finished last Frame
	        Changed = false;	// Reset Checkbox was changed last frame
	g_State.IsPress = false;
	// если кнопка активна
	if (Enabled == true) 	
	{

		g_State.IsMouseOnButton = false;
		// Если объект CHeckBox и установлен
		if ( ObjectType == CHECKBOX && Checked )
			g_State.IsPress = true;
		else
			g_State.IsPress = false;

		// Пpоверяем попала ли мышка в объект
		if ((( InputClass.MousePos.x > ABSoluteX && InputClass.MousePos.x < ABSoluteX + ABSolute_Width ) &&
			 ( InputClass.MousePos.y > ABSoluteY && InputClass.MousePos.y < ABSoluteY + ABSolute_Height )))
		{
			if ( InputClass.m_mouseState.rgbButtons[0] )  // Мышка была нажата на объекте
			{
				


				switch ( ObjectType )
				{
					case BUTTON:
								NeedReleaseOnObject = BUTTON;
								BUSY_Object_Index = ObjectIndex;
								break;

					// CheckBox Если кнопка с фиксацией
					case CHECKBOX:
								NeedReleaseOnObject = CHECKBOX;
								BUSY_Object_Index = ObjectIndex;
								break;
					// Label
					case LABEL:
								NeedReleaseOnObject = LABEL;
								BUSY_Object_Index = ObjectIndex;
								break;
					// Edit
					case EDIT:
								NeedReleaseOnObject = EDIT;
								BUSY_Object_Index = ObjectIndex;
								break;
					case COLOR_PICK:
								NeedReleaseOnObject = COLOR_PICK;
								BUSY_Object_Index = ObjectIndex;
								break;
					default:;				
				}

				if ( IsButonPressTexture != NULL || Checked )
				{
					Body->Buffers->RenderTexture = IsButonPressTexture;	// новая попытка )
				}
				else
					Body->Buffers->RenderTexture = OsnButtontexture;	// новая попытка )
			}
			else // Мышка не была нажата на объекте
			{ 
				switch ( ObjectType )
				{
					// Button
					case BUTTON:
						if (NeedReleaseOnObject == BUTTON )
						{
							NeedReleaseOnObject = -1;
							BUSY_Object_Index = -1;
							g_State.IsPress = true; // Если без фиксации					
						}
						break;

					// CheckBox
					case CHECKBOX:
						if ( NeedReleaseOnObject == CHECKBOX )
						{
							NeedReleaseOnObject = -1;
							BUSY_Object_Index = -1;
							g_State.IsPress = true;
							Changed = true;

							Checked ? Checked = false :	Checked = true;
						}
						break;

					// Label
					case LABEL:
						if (NeedReleaseOnObject == LABEL )
						{
							NeedReleaseOnObject = -1;
							BUSY_Object_Index = -1;

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
							BUSY_Object_Index = -1;
							ActiveAsEdit = true;
							ActiveEditChanged = true;
						}

					// COLOR
					case COLOR_PICK:
					if ( NeedReleaseOnObject == COLOR_PICK )
					{
						NeedReleaseOnObject = -1;
						BUSY_Object_Index = -1;
						g_State.IsPress = true;

						SetColour ? SetColour = false : SetColour = true;

					}

				default:;
				}

				if (IsMouseOnButtonTexture != NULL)
				{
					Body->Buffers->RenderTexture = IsMouseOnButtonTexture;
				}
				else 
				{
					if ( Checked )
						Body->Buffers->RenderTexture = IsButonPressTexture;
					else
						Body->Buffers->RenderTexture = OsnButtontexture;
				}
			}	

			g_State.IsMouseOnButton = true;

		}
		else
		{
			// Сбрасываем нажатие на объект, если кнопку мыши отпустили сместив мышку с объекта
			if ( BUSY_Object_Index == ObjectIndex && !InputClass.m_mouseState.rgbButtons [0] )
			{
				NeedReleaseOnObject = -1;
				BUSY_Object_Index = -1;
			}


			if ( Checked )
			{
				Body->Buffers->RenderTexture = IsButonPressTexture;
			}
			else
			{
				Body->Buffers->RenderTexture = OsnButtontexture;
			}

		}
	} // если кнопка НЕ активна

	return ActiveEditChanged;
}


void KFButton::ConstructBody ( XMFLOAT4 &BobyPos )
{
	BobyPos.x = FormCoord.x + ObjParam.x;
	BobyPos.y = FormCoord.y + ObjParam.y;
	BobyPos.z = ObjParam.z;
	BobyPos.w = ObjParam.w;
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
		Body->Buffers->RenderTexture = OsnButtontexture;
	}
	else
	{
		Enabled = false;
		if( NotEnalbledTexture != nullptr )
		{
			Body->Buffers->RenderTexture = NotEnalbledTexture;
		}
		else
		{
			Body->Buffers->RenderTexture = OsnButtontexture;
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

		Body->GenerateVertexes ( vertexes, COLOR_BUTTON, Colour );

		Body->Buffers->FlatObjectVB->Update ( vertexes );

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

		Body->GenerateVertexes ( vertexes, COLOR_BUTTON, RGBA.Fl4 );

		Body->Buffers->FlatObjectVB->Update ( vertexes );

		delete [] vertexes;
	}
}


XMFLOAT4 KFButton::GetButtonColor ()
{
	return Colour;
}
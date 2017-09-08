#include "stdafx.h"
#include "MenuControler.h"

MenuControrerClass::MenuControrerClass(){

//	memset(this, 0, sizeof(MenuControrerClass));
				g_Device = nullptr;
		 g_DeviceContext = nullptr;
				 McD3DGC = nullptr;
			  Background = nullptr;
	// ������� �� ���� - � �� �������� ������
			IsMenuActive = false;
	// �������� �� ����
			  IsMenuDraw = false;
			  CursorTime = 0.0f;
			  KeyPressed = false;
			 TempKeyCode = -1;
   LastActiveButtonIndex = -1;
		ObjectActiveBUSY = false;
	   KeyDefineExpected = false;
}

HRESULT MenuControrerClass::Init(D3DGlobalContext* D3DGC,
	KFButton_Elements ButtonsData[], int NumOfButtons, 
	KFScrollBar_Elements ScrollBarData[], int NumOfScrollbars, 
	StringsList_Elements StringsLists[], int NumOfStringsList,
	KF2DTextureAnimation *_Animation,
	XMFLOAT4 BackGroundCoord,
	ID3D11ShaderResourceView* BackgroundTexture,
	ID3D10Blob* MenuDrawPlaceBlob,
	TextClass* GolobalText 
	)
{
	HRESULT result = S_OK ;
	
		McD3DGC = D3DGC;
		 G_Text = GolobalText;
	GlobalInput = D3DGC->m_EngineInputClass;
	
	g_WindowPosX = D3DGC->WindowsPosX;
	g_WindowPosY = D3DGC->WindowsPosY;
	MWidth = (int)BackGroundCoord.z;
	MHeigth = (int)BackGroundCoord.w;
//	g_hwnd = hwnd ;
	
	m_depthStens = D3DGC->m_depthStencilView;

	g_NumOfButtons = NumOfButtons ;
	// ����������� ����� ��� ������
	Buttons.reserve( NumOfButtons );

	g_NumOfScrollBars = NumOfScrollbars ;
	ScrollBars.reserve ( NumOfScrollbars );

	g_NumOfStringsLists = NumOfStringsList;

	g_Device = D3DGC->D11_device;
	g_DeviceContext = D3DGC->D11_deviceContext ;

	Animation = _Animation;

	Background = new SquareObjectClass;
	Background->Init(McD3DGC, BackGroundCoord,
		BackgroundTexture, 0, MenuDrawPlaceBlob);

	// ������������� �������� �� ��� ����
	if ( Animation )
		Animation->SetAnimeParam ( BackGroundCoord );

	XMFLOAT4 ScrCoords =
	{
		(float)McD3DGC->WindowsPosX, // ������� ���� X
		(float)McD3DGC->WindowsPosY, // ������� ���� Y
		(float)McD3DGC->ScreenWidth, // ������ ������ ������
		(float)McD3DGC->ScreenHeight // ������ ������ ������ 
	};

	KFButton* TempButton;

	int c = 0;
	while(c < NumOfButtons)
	{

		TempButton = new KFButton ;
		Buttons.emplace_back (TempButton);

		KFButton_Elements* ButtonInitData = &ButtonsData[c];

		// ���� � Button ����������� �����, �� ������� ��� ������ ��� �������� ��������� ������
		// ������� ����� ���������� ������ ������
		int LabelSize = (int)strlen( ButtonInitData->Label );

		int SentenceIndex = -1;

		// ���� �������� ������ Edit ��� Label, �� Label ����� ���� NULL, �� ButtonsData[c].Data->MaxLength ����������� ������ ���� ������ 0 
		// ����� Edit � Label ��������� �������
//		if ( ( LabelSize != NULL && ButtonInitData->Data != NULL ) || ((ButtonInitData->Data != NULL && ButtonInitData->Data->MaxLength > 0 ) && ( ButtonInitData->Type == LABEL || ButtonInitData->Type == EDIT )) )
		if (( ButtonInitData->Type == LABEL || ButtonInitData->Type == EDIT ) && 
		   (  ButtonInitData->Data != NULL ))
		{
//			ButtonInitData->Data->MaxLength < 0 ? ButtonInitData->Data->MaxLength = 2 : ButtonInitData->Data->MaxLength;
			SentenceIndex = GolobalText->AddSentence( ButtonInitData->Data, ButtonInitData->Label );
			if ( SentenceIndex == -1 )
				assert("������������� �������� ������ �� ������.");
		}
		else
		{

		}

		ButtonInitData->SentenceIndex = SentenceIndex; // ���� ���� ������ � ��������, �� ����������� � ������ � ������� � ��������


		TempButton->Init(
			D3DGC,
			ScrCoords, 
			BackGroundCoord,
			*ButtonInitData
			);

		TempButton->SetIfButtonPressTexture( ButtonInitData->IsClickTexture);
		TempButton->SetIfMouseOnButtonTexture( ButtonInitData->IsMouseOnButtonTexture );
		TempButton->SetIfButtonNotEnalbledTexture( ButtonInitData->IsNotEnalbledTexture );
			
		++c ;
	}
	
	ScrollBarClass* TempScrollBar;

	// �������� ������ �����
    c = 0 ;
	while (c < NumOfScrollbars)
	{
		TempScrollBar = new ScrollBarClass;
		ScrollBars.emplace_back ( TempScrollBar );

		KFScrollBar_Elements* TempScrollBarData = &ScrollBarData[c];


			TempScrollBar->Init(
			D3DGC,
			ScrCoords,
			BackGroundCoord,
			*TempScrollBarData
			);

			TempScrollBar->SetMouseOnButtonTexture( TempScrollBarData->MouseOnButtonTexture);
			TempScrollBar->SetMouseOnTravellerTexture( TempScrollBarData->MouseOnTravellerTexture );
			TempScrollBar->SetBarNotEnalbledTexture( TempScrollBarData->BodyNotEnalbledTexture );
			TempScrollBar->SetBarNotEnalbledButtonsTexture( TempScrollBarData->ButtonNotEnalbledTexture );
			TempScrollBar->SetTravellerNotEnalbledTexture( TempScrollBarData->TravellerNotEnalbledTexture );
			TempScrollBar->SetButtonsPressTexture( TempScrollBarData->ButtonPressTexture );
			TempScrollBar->SetTravellerPressTexture( TempScrollBarData->TravellerPressTexture );
			
			++c;
	}

	StringsListClass* TempStringsList;

	c = 0;
	while ( c < NumOfStringsList )
	{

		TempStringsList = new StringsListClass;
		StringsList.push_back( TempStringsList );

		StringsList_Elements* TempStringsListData = &StringsLists[c];

		TempStringsList->Init(
			D3DGC,
			ScrCoords,
			BackGroundCoord,
			*TempStringsListData,
			G_Text
		);
		
		++c;
	}


	g_MenuPosX = BackGroundCoord.x;
	g_MenuPosY = BackGroundCoord.y;
	g_Width = MWidth ;
	g_Heigth = MHeigth ;

	Disable = D3DGC->m_alphaDisableBlendingState;

	return result;
}


void MenuControrerClass::Draw()
{
		ElementsRender();
}


MenuControrerClass::~MenuControrerClass(){
	int c = 0;

	RCUBE_DELETE ( Background );

	// ������� ��� ������
	int size = (int)ScrollBars.size();
	for (int c = 0; c < size; ++c)
	{
		delete ScrollBars[c];
	}
	ScrollBars.clear();

	// ������� ��� ������
	size = (int)Buttons.size();
	for(int c = 0 ; c < size ; ++c )
	{
		delete Buttons[c];
	}
	Buttons.clear();

	// ������� ��� StringLists
	size = ( int ) StringsList.size();
	for ( int c = 0; c < size; ++c )
	{
		delete StringsList[c];
	}
	StringsList.clear();
	// --------------------------------------------------

}

void MenuControrerClass::UpdateObjectText ( KFButton* TempButton, char* Str )
{
	// �������� ������ ������ � ��������, ����� ����� ���� ��� ��������� �������� ������� ����������� ������
	long TextHight = ( long ) G_Text->GetSentenceHeight ( TempButton->SentenceIndex ) / 4;

	G_Text->UpdateSentence ( TempButton->SentenceIndex, Str, int ( TempButton->ABSoluteX + 10 ),
							 int ( TempButton->ABSoluteY + ( TempButton->ABSolute_Height - TextHight ) / 2 ) );
}


HRESULT MenuControrerClass::Frame( InputClass* InputClass, int UTF16_KeyCODE, FPSTimers &fpstimers)
{
	// ��� ���������� ��� ������� ��� ���������� ���� ������ ����� ������ IsMenuActive
	if ( IsMenuActive )
	{
		// ������ ��� �������� ������� � �������� 0.5 - ��� �������
		if (fpstimers.FpsRate > 0 )
			CursorTime +=  1 / float( fpstimers.FpsRate );

		if ( CursorTime > 0.5f ) //  ������� 0.5f �������
		{
			CursorTime = 0.0f;

			if ( CursorOn_Off )
				CursorOn_Off = false;
			else
				CursorOn_Off = true;
		}

// ��������� ���� ������ �� �����
		int c = 0;
		KFButton* TempButton;
		
		while (c < g_NumOfButtons)
		{
			TempButton = Buttons[c];
				 // FRAME ��� ������� BUTTON
			if ( TempButton->Frame( InputClass->DxInputStruct, ObjectActiveBUSY ) )
			{
				if (LastActiveButtonIndex > -1 )
				// �������� ������ � ���������� �������� Edit, ���� �� ��� �������
				SwithOFFCursor( Buttons[LastActiveButtonIndex] );

				ChangeActiveEdit( c );
				LastActiveButtonIndex = c;
			}
			// ���� ������� Edit �������, �� ������ � �� ������
			// ���� ������ Label �������, �� ��� � ������� �������
			if ( TempButton->ActiveAsEdit )
				// ����� ��� �������� ���� �������� - ���� ������ ������ ���� �� Edit ��� Label
				switch ( TempButton->ObjectType )
				{
					// ���� Label, �� ��������� ����� �� ������� ������� �� ����� ������
					case 2:
							// ���� ����� ������ ������, �� �������� �������� ������� �� ����� ������
							if ( TempButton->WaitingForKeyPress )
							{
									UpdateObjectText ( TempButton,
									GlobalInput->GetVKeyText ( TempButton->VirtualKeyIndex, TempButton->SecondSlot )
									 );

									int kode = InputClass->GetPressedKey();

									if ( kode != -1 )
									{
										InputClass->BindKey( 0, (UINT)kode, true );
										TempButton->ActiveAsEdit = false;
										UpdateObjectText ( TempButton,
										GlobalInput->GetVKeyText ( TempButton->VirtualKeyIndex, TempButton->SecondSlot )
										);
									}


							}
							break;

					// ���� Edit, �� �������� ������� �������������� ������ ����������� � Edit
					case 3:
						{
						int MAXLength = G_Text->GetSentenceMaxLength( TempButton->SentenceIndex );
						// �������� ������ ������ � ��������, ����� ����� ���� ��� ��������� �������� ������� ����������� ������
//						long TextHight = (long)G_Text->GetSentenceHeight( TempButton->SentenceIndex ) / 4;

						char* TempString = new char [MAXLength];

						strcpy_s( TempString, (size_t)MAXLength, G_Text->GetSentenceText( TempButton->SentenceIndex ) );

						int RealLength = (int)strlen( TempString );

						// ++++++++++++++++++++++++++++++    ������� ��������     +++++++++++++++++++++++++++++++++++++++++
						// ���� ������ ���
						if ( CursorOn_Off )
						{
							if ( !TempButton->CursorOn )
							{
								if ((RealLength + 1) < MAXLength )
								{
									TempButton->CursorOn = true;
									strcat_s( TempString, MAXLength, "|" );
									UpdateObjectText ( TempButton, TempString);
								}
							}
						}
						// ���� ������ ����
						else
						{
							if ( TempButton->CursorOn )
							{
								TempButton->CursorOn = false;
								TempString[RealLength - 1] = NULL;
								UpdateObjectText ( TempButton, TempString );
							}
						}
						// ++++++++++++++++++++++++++++++    ������� ��������     +++++++++++++++++++++++++++++++++++++++++

					// ���� �������� ������ ������������, �� ��������� ��� � �����
					// ��� ���� ����� ������ ����� ���� ��������� � ��������� �� ������ �������
					if (UTF16_KeyCODE != -1 && !KeyPressed )
					{
						KeyPressed = true;
						TempKeyCode = UTF16_KeyCODE;
					}

					// ��� ���� ����� ������ ����� ���� ��������� � ��������� �� ������ �������
					if ( KeyPressed && UTF16_KeyCODE == -1 )
					{
						KeyPressed = false;

						// ��������� �������� ������ ������
						RealLength = (int)strlen( TempString );

						// Enter
						if (TempKeyCode == 13)
						{
							// ���� ������ � ������ - ������� ���.
							if (TempButton->CursorOn)
							{
								TempButton->CursorOn = false;
								TempString[RealLength - 1] = NULL;
							}

							TempButton->ActiveAsEdit = false;
							UpdateObjectText ( TempButton, TempString );
							break;
						}

						// ���� ������ ������ ������ ������������
						if (RealLength < MAXLength - 2 || TempKeyCode == 8) // -2  ������ ��� ����� ����� ��� ������� + � ����� ������ ��� /0
						{

							// ���� ������ � ������ - ������� ���.
							if (TempButton->CursorOn)
							{
								TempButton->CursorOn = false;
								TempString[RealLength - 1] = NULL;
							}

							char Str[2];
							// ��� ��������� Edit ������� �� ��������
							switch ( TempButton->EditType )
							{
								case ANYSYMBOL:
							
									Str[0] = UTF16toCHAR ( TempKeyCode );
									Str[1] = NULL;
									break;

								case AS_INTEGER:

									Str[0] = UTF16toINT ( TempKeyCode );
									Str[1] = NULL;

									// ��������� ��� ����� ����� ��������� ������ � ������ ������
									if (Str[0] == 0x2D)
										if ( !strlen( TempString ) == NULL )
										{
											Str[0] = 0x00;
											Str[1] = NULL;
										}

									break;

								case AS_FLOAT:
									Str[0] = UTF16toFLOAT ( TempKeyCode );
									Str[1] = NULL;
									// �������� �� ������������ ������ ����� � �����
									if ( Str[0] == 0x2E )
										if ( PointSet( TempString ) )
										{
											Str[0] = 0x00;
											Str[1] = NULL;
										}
									// ��������� ��� ����� ����� ��������� ������ � ������ ������
									if ( Str[0] == 0x2D )
										if ( !strlen( TempString ) == NULL )
										{
											Str[0] = 0x00;
											Str[1] = NULL;
										}


								default:;
							}

							// ���� ����� ������� ������ ( BackSpace ����� )
							if (Str[0] == 8)
							{
								RealLength = (int)strlen( TempString );
								// ������� ������ ������ ���� ������ �������� ���� ���� ������
								if (RealLength > 0)
								{
									RealLength = (int)strlen( TempString );
									TempString[RealLength - 1] = NULL;
								}
							}
							else
							{
								strcat_s( TempString, MAXLength, Str );
							}

							UpdateObjectText ( TempButton, TempString );
						}
					}

					delete [] TempString;
					break;// case 3
					}
			default:;
		}	// switch END

		++c;
		}

// ScrollBars
		c = 0;
		while (c < g_NumOfScrollBars)
		{
			ScrollBars[c]->Frame( InputClass->DxInputStruct, fpstimers, ObjectActiveBUSY );
			++c;
		}

// StringsList
		c = 0;
		while ( c < g_NumOfStringsLists )
		{
			StringsList[c]->Frame( InputClass->DxInputStruct, fpstimers, ObjectActiveBUSY );
			++c;
		}
	}

	if ( Animation )
		Animation->Frame( fpstimers );

	return S_OK ;
}


void MenuControrerClass::ChangeActiveEdit( int ActiveNumber )
{
	int c = 0;
	while (c < g_NumOfButtons)
	{
		if ( c != ActiveNumber )
			Buttons[c]->ActiveAsEdit = false;

		++c;
	}
}


void MenuControrerClass::SetButtonAnimation(int NumOfVerses , int NumofColom , ID3D11ShaderResourceView * KF2DTextureFileName
	, int IndexOfButton , int FramesSpead){

//	Buttons[IndexOfButton]->SetIsMouseOnButtonAnimation(NumOfVerses , NumofColom , KF2DTextureFileName , 20) ;

}

void MenuControrerClass::ElementsRender(){

	if ( Animation )
		Animation->Draw();


	// ���� ���� ������ ��� 
	if (Background != nullptr)
	{
		Background->Draw();
	}

	int c = 0 ;
	while (c < g_NumOfButtons)
	{
		Buttons[c]->Draw();	
		++c ;
	}
	 
	c = 0;
	while (c < g_NumOfScrollBars)
	{
		ScrollBars[c]->Draw() ;
		++c ;
	}
	
	c = 0;
	while ( c < g_NumOfStringsLists )
	{
		StringsList[c]->Draw();
		++c;
	}
}


void MenuControrerClass::SetButtonPos( int ButtonIndex, float x, float y )
{
	KFButton* TButton;
	TButton = Buttons[ButtonIndex];
	TButton->ObjParam.x = x;
	TButton->ObjParam.y = y;
	TButton->ResetButtonParam();
//	TButton->UpdateABSElementPos();
}


void MenuControrerClass::SetButtonPos(int ButtonIndex, float Percent )
{
	KFButton* TButton;
	TButton = Buttons[ButtonIndex];
	TButton->ObjParam.x = TButton->ObjOriginalParam.x * Percent;
	TButton->ObjParam.y = TButton->ObjOriginalParam.y * Percent;
	TButton->ResetButtonParam();
//	TButton->UpdateABSElementPos();
}


void MenuControrerClass::SetButtonSize( int ButtonIndex, float width , float heigth )
{
	KFButton* TButton;
	TButton = Buttons[ButtonIndex];
	TButton->ObjParam.z = width;
	TButton->ObjParam.w = heigth;
	TButton->ResetButtonParam();
	TButton->UpdateABSElementSize();
}


void MenuControrerClass::SetButtonSize(int ButtonIndex, float Percent)
{
	KFButton* TButton;
	TButton = Buttons[ButtonIndex];
	TButton->ObjParam.z = TButton->ObjOriginalParam.z * Percent;
	TButton->ObjParam.w = TButton->ObjOriginalParam.w * Percent;
	TButton->ResetButtonParam();
	TButton->UpdateABSElementSize();
}


void MenuControrerClass::SetButtonParam(int ButtonIndex, XMFLOAT4& Param)
{
	KFButton* TButton;
	TButton = Buttons[ButtonIndex];
	TButton->ObjParam = Param;
	TButton->ResetButtonParam();
	TButton->UpdateABSElementAll();
}


void MenuControrerClass::SetButtonToOrigin( int ButtonIndex )
{
	KFButton* TButton;
	TButton = Buttons[ButtonIndex];
	TButton->ObjParam = TButton->ObjOriginalParam;
	TButton->ResetButtonParam();
	TButton->UpdateABSElementAll();
}


void MenuControrerClass::SetMenuPos(XMFLOAT2& Pos)
{
	Background->ObjParam.x = Pos.x;
	Background->ObjParam.y = Pos.y;
	Background->SetObjParam();
	UpdateMenuElementsPos();
}


void MenuControrerClass::UpdateMenuElementsPos()
{
	KFButton* TButton;
	int c = 0;
	while (c < g_NumOfButtons)
	{
		TButton = Buttons[c];
		TButton->FormCoord = Background->ObjParam;
		TButton->ResetButtonParam();
		++c;
	}
}


void  MenuControrerClass::SetMenuSize( float Percent )
{
	Background->ObjParam.z = Background->ObjOriginalParam.z * Percent;
	Background->ObjParam.w = Background->ObjOriginalParam.w * Percent;
	Background->SetObjParam();
	UpdateMenuElementsAll();
}


void MenuControrerClass::UpdateMenuElementsAll()
{
	KFButton* TButton;
	int c = 0;
	while (c < g_NumOfButtons)
	{
		TButton = Buttons[c];
		TButton->FormCoord = Background->ObjParam;
		TButton->ResetButtonParam();
		TButton->UpdateABSElementAll();
		++c;
	}
}


void MenuControrerClass::AddAnyMenuEffect(int Index)
{


}


void MenuControrerClass::SetMenuActive(bool Value)
{

	IsMenuActive = Value;

}


void MenuControrerClass::PutLabelsInsideButtons()
{
	int c = 0;
	int NumOfButtons = (int)Buttons.size();
	while (c < NumOfButtons) 
	{
		KFButton* TempButton = Buttons[c];

		// ���� � Button ����������� �����, �� ������� ��� ������ ��� �������� ��������� ������
		// ������� ����� ���������� ������ ������
		if (TempButton->SentenceIndex > -1 )
		{
//			int LabelSize = G_Text->GetSentenceLength( Buttons[c]->SentenceIndex );

		UpdateObjectText ( TempButton, G_Text->GetSentenceText(TempButton->SentenceIndex) );
		
		}

		++c;
	}
}


void MenuControrerClass::PutLabelInsideButton( int ButtonIndex )
{
	KFButton* TempButton = Buttons[ ButtonIndex ];

	if (TempButton->SentenceIndex > -1)
	{

		UpdateObjectText ( TempButton, G_Text->GetSentenceText(TempButton->SentenceIndex));

	}
}


char MenuControrerClass::UTF16toCHAR ( int UTF16_KeyPressed )
{
	// Enter
	if  ( UTF16_KeyPressed == 13 )
		return 13;
	// Backspace
	if ( UTF16_KeyPressed == 8 )
		return 8;
	// �
	if ( UTF16_KeyPressed == 0x0401 )
		return -88;
	// �
	if (UTF16_KeyPressed == 0x0451)
		return -72;
	// Cyrillic
	if ( UTF16_KeyPressed > 0x040F && UTF16_KeyPressed < 0x0450 )
		return char( UTF16_KeyPressed - 0x350 );
	// English
	if ( UTF16_KeyPressed > 0x001F && UTF16_KeyPressed < 0x0080 )
	return (char)UTF16_KeyPressed;

	return NULL;
}


char MenuControrerClass::UTF16toINT ( int UTF16_KeyPressed )
{
	// Enter
	if ( UTF16_KeyPressed == 13 )
		return 13;
	// Backspace
	if ( UTF16_KeyPressed == 8 )
		return 8;
	// Minus
	if ( UTF16_KeyPressed == 0x002D )
		return 0x2D;
	// Digits
	if ( UTF16_KeyPressed > 0x002F && UTF16_KeyPressed < 0x003A )
		return char( UTF16_KeyPressed );

	return NULL;
}

// http://cppstudio.com/post/812/
char MenuControrerClass::UTF16toFLOAT ( int UTF16_KeyPressed )
{
	// Enter
	if ( UTF16_KeyPressed == 13 )
		return 13;
	// Backspace
	if ( UTF16_KeyPressed == 8 )
		return 8;
	// Point
	if ( UTF16_KeyPressed == 0x002E )
		return 0x2E;
	// Minus
	if ( UTF16_KeyPressed == 0x002D )
		return 0x2D;
	// Digits
	if ( UTF16_KeyPressed > 0x002F && UTF16_KeyPressed < 0x003A )
		return char( UTF16_KeyPressed  );

	return NULL;
}


bool MenuControrerClass::PointSet ( char* Str )
{
	int Length = (int)strlen ( Str );
	
	if (Length == 0)
		return false;

	for ( int i = 0; i <= Length; ++i )
	{
		if ( Str[i] == 0x2E )
			return true;
	}

	return false;
}


void MenuControrerClass::SwithOFFCursor ( KFButton* Button )
{
	KFButton* TempButton;

	TempButton = Button;

	if ( TempButton->CursorOn == true )
	{
		TempButton->CursorOn = false;

		int MAXLength = G_Text->GetSentenceMaxLength( TempButton->SentenceIndex );
		// �������� ������ ������ � ��������, ����� ����� ���� ��� ��������� �������� ������� ����������� ������
//		long TextHight = (long)G_Text->GetSentenceHeight( TempButton->SentenceIndex ) / 4;

		char* TempString = new char[MAXLength];

		strcpy_s( TempString, (size_t)MAXLength, G_Text->GetSentenceText( TempButton->SentenceIndex ) );

		int RealLength = (int)strlen( TempString );

		TempString[RealLength - 1] = NULL;
	
		UpdateObjectText ( TempButton, TempString );

		delete [] TempString;
	}
}


float MenuControrerClass::GetScrollBarValue( int ScrBarIndex )
{
	int Size = (int)ScrollBars.size();
	if ( ScrBarIndex < Size )
		return ScrollBars[ScrBarIndex]->GetCurrentValue();
	else
		return -666.66666666f;

}


void MenuControrerClass::SetScrollBarEnable( int ScrBarIndex, bool value )
{
	int Size = ( int ) ScrollBars.size();

	if ( ScrBarIndex < Size )
		ScrollBars[ScrBarIndex]->SetEnable( value );

}


bool MenuControrerClass::SetScrollBarValue( int ScrBarIndex, float &Value )
{
	int Size = ( int ) ScrollBars.size();

	if ( ScrBarIndex < Size )
	{
		ScrollBars[ScrBarIndex]->SetValue( Value );
		return true;
	}
		return false;
}


bool MenuControrerClass::GetButtonState( int ButtonIndex )
{
	int Size = (int)Buttons.size();

	ButtonState State;
	if ( ButtonIndex < Size )
	{
		Buttons[ButtonIndex]->GetButtonState( State );
		return State.IsPress;
	}

	return false;
}


void MenuControrerClass::ClearButtonState( int ButtonIndex )
{
	int Size = (int)Buttons.size();

	if( ButtonIndex < Size )
		Buttons[ButtonIndex]->ClearButtonState();
}


void MenuControrerClass::SetButtonEnable( int ButtonIndex, bool value )
{
	int Size = (int)Buttons.size();

	if( ButtonIndex < Size )
		Buttons[ButtonIndex]->SetEnable( value );

}


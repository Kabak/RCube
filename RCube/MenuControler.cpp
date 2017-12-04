#include "stdafx.h"
#include "MenuControler.h"

MenuControrerClass::MenuControrerClass(){

//	memset(this, 0, sizeof(MenuControrerClass));
//				g_Device = nullptr;
//		 g_DeviceContext = nullptr;
				 McD3DGC = nullptr;
			  Background = nullptr;
	// Активно ли меню - в нём работают кнопки и его рисуем
			IsMenuActive = false;
	// Рисовать ли меню
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
	ResourceManager * GlobalResManager
	)
{
	HRESULT result = S_OK ;
	
		McD3DGC = D3DGC;
	GlobalInput = D3DGC->m_EngineInputClass;
	GlobalResourceManager = GlobalResManager;
	g_WindowPosX = D3DGC->WindowsPosX;
	g_WindowPosY = D3DGC->WindowsPosY;
	MWidth = (int)BackGroundCoord.z;
	MHeigth = (int)BackGroundCoord.w;

	g_NumOfButtons = NumOfButtons ;
	// резервируем место под кнопки
	Buttons.reserve( NumOfButtons );

	g_NumOfScrollBars = NumOfScrollbars ;
	ScrollBars.reserve ( NumOfScrollbars );

	g_NumOfStringsLists = NumOfStringsList;

//	g_Device = D3DGC->DX_device;
//	g_DeviceContext = D3DGC->DX_deviceContext ;

	Animation = _Animation;

	int TempIndex = GlobalResourceManager->Create_Flat_Obj_Buffers ( D3D11_USAGE_DEFAULT, 4, 6, BackgroundTexture );
	Background = new FlatObjectClass;
	Background->Init( D3DGC->ScreenWidth, D3DGC->ScreenHeight, BackGroundCoord,
		BackgroundTexture, 
		NO_FLIP,
		GlobalResourceManager->Get_Flat_ObjectBuffers_ByIndex ( TempIndex )
	);

	// Устанавливаем анимацию на фон меню
	if ( Animation )
		Animation->SetAnimeParam ( BackGroundCoord );

	XMFLOAT4 ScrCoords =
	{
		(float)McD3DGC->WindowsPosX, // Позиция окна X
		(float)McD3DGC->WindowsPosY, // Позиция окна Y
		(float)McD3DGC->ScreenWidth, // Размер экрана ширина
		(float)McD3DGC->ScreenHeight // Размер экрана высота 
	};

	KFButton* TempButton;

	int c = 0;
	while(c < NumOfButtons)
	{

		TempButton = new KFButton ;
		Buttons.emplace_back (TempButton);
		KFButton_Elements* ButtonInitData = &ButtonsData[c];
		TempButton->BuffersIndex = GlobalResourceManager->Create_Flat_Obj_Buffers ( D3D11_USAGE_DEFAULT, 4, 6, ButtonsData[c].OsnTexture );

		// Если к Button присоединен текст, то смотрим его длинну для создания текстовой строки
		// Которая будет рисоваться поверх кнопки
		int LabelSize = (int)strlen( ButtonInitData->Label );

		int SentenceIndex = -1;

		// Если создатся объект Edit или Label, то Label можут быть NULL, но ButtonsData[c].Data->MaxLength обязательно должно быть больше 0 
		// чтобы Edit и Label корректно работал
//		if ( ( LabelSize != NULL && ButtonInitData->Data != NULL ) || ((ButtonInitData->Data != NULL && ButtonInitData->Data->MaxLength > 0 ) && ( ButtonInitData->Type == LABEL || ButtonInitData->Type == EDIT )) )
		if (( ButtonInitData->Type == LABEL || ButtonInitData->Type == EDIT ) && 
		   (  ButtonInitData->Data != NULL ))
		{
//			ButtonInitData->Data->MaxLength < 0 ? ButtonInitData->Data->MaxLength = 2 : ButtonInitData->Data->MaxLength;
			SentenceIndex = GlobalResourceManager->AddSentence( ButtonInitData->Data, ButtonInitData->Label );
			if ( SentenceIndex == -1 )
				assert("Инициализация названия кнопки не прошла.");
		}
		else
		{

		}

		ButtonInitData->SentenceIndex = SentenceIndex; // Если есть строка у элемента, то привязываем её индекс в массиве к элементу


		TempButton->Init(
			D3DGC,
			ScrCoords, 
			BackGroundCoord,
			*ButtonInitData,
			GlobalResourceManager->Get_Flat_ObjectBuffers_ByIndex( TempButton->BuffersIndex )
			);

		TempButton->SetIfButtonPressTexture( ButtonInitData->IsClickTexture);
		TempButton->SetIfMouseOnButtonTexture( ButtonInitData->IsMouseOnButtonTexture );
		TempButton->SetIfButtonNotEnalbledTexture( ButtonInitData->IsNotEnalbledTexture );
		TempButton->SetOsnTexture ( ButtonInitData->OsnTexture );
			
		++c ;
	}
	
	ScrollBarClass* TempScrollBar;

	// создание скролл баров
    c = 0 ;
	while (c < NumOfScrollbars)
	{
		TempScrollBar = new ScrollBarClass;
		ScrollBars.emplace_back ( TempScrollBar );

		KFScrollBar_Elements* TempScrollBarData = &ScrollBarData[c];
		int IndexMinButton = GlobalResourceManager->Create_Flat_Obj_Buffers ( D3D11_USAGE_DEFAULT, 4, 6, TempScrollBarData->ButtonsTexture );
		int IndexMaxButton = GlobalResourceManager->Create_Flat_Obj_Buffers ( D3D11_USAGE_DEFAULT, 4, 6, TempScrollBarData->ButtonsTexture );
		int IndexMinTraveler = GlobalResourceManager->Create_Flat_Obj_Buffers ( D3D11_USAGE_DEFAULT, 4, 6, TempScrollBarData->TravellerTexture );
		int Body = GlobalResourceManager->Create_Flat_Obj_Buffers ( D3D11_USAGE_DEFAULT, 4, 6, TempScrollBarData->BodyTexture );

			TempScrollBar->Init(
			D3DGC,
			ScrCoords,
			BackGroundCoord,
			*TempScrollBarData,
			GlobalResourceManager->Get_Flat_ObjectBuffers_ByIndex( IndexMinButton ),
			GlobalResourceManager->Get_Flat_ObjectBuffers_ByIndex ( IndexMaxButton ),
			GlobalResourceManager->Get_Flat_ObjectBuffers_ByIndex ( IndexMinTraveler ),
			GlobalResourceManager->Get_Flat_ObjectBuffers_ByIndex ( Body )
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
		int TempIndex = GlobalResourceManager->Create_Flat_Obj_Buffers ( D3D11_USAGE_DEFAULT, 4, 6, nullptr );

		TempStringsList->Init(
			D3DGC,
			ScrCoords,
			BackGroundCoord,
			*TempStringsListData,
			GlobalResourceManager,
			GlobalResourceManager->Get_Flat_ObjectBuffers_ByIndex ( TempIndex )
		);
		
		++c;
	}


	g_MenuPosX = BackGroundCoord.x;
	g_MenuPosY = BackGroundCoord.y;
	g_Width = MWidth ;
	g_Heigth = MHeigth ;

	return result;
}


MenuControrerClass::~MenuControrerClass()
{

}

// Размещает текст поверх элементов меню в центе элемента по Y
void MenuControrerClass::UpdateObjectText ( KFButton* TempButton, char* Str )
{
	int YPos;
	// Получаем высоту текста в пикселях, чтобы можно было его выровнять поцентру объекта содержащего строку
	long TextHight = ( long )GlobalResourceManager->GetSentenceHeight ( TempButton->SentenceIndex );

	if (TempButton->ABSolute_Height > TextHight)
		YPos = (int)TempButton->ABSoluteY + (TempButton->ABSolute_Height - TextHight) / 2 ;
	else
		YPos = (int)TempButton->ABSoluteY + (TextHight - TempButton->ABSolute_Height) / 2;
	
	GlobalResourceManager->UpdateSentence ( TempButton->SentenceIndex, Str, int ( TempButton->ABSoluteX + 10 ), YPos );
}


HRESULT MenuControrerClass::Frame( InputClass* InputClass, int UTF16_KeyCODE, FPSTimers &fpstimers)
{
	// Все происходит при слловии что конкретное меню сейчас имеет статус IsMenuActive
	if ( IsMenuActive )
	{
		// Таймер для моргания курсора с частотой 0.5 - пол секунды
		if (fpstimers.FpsRate > 0 )
			CursorTime +=  1 / float( fpstimers.FpsRate );

		if ( CursorTime > 0.5f ) //  Частота 0.5f секунда
		{
			CursorTime = 0.0f;

			if ( CursorOn_Off )
				CursorOn_Off = false;
			else
				CursorOn_Off = true;
		}

// Обработка всех кнопок на форме
		int c = 0;
		KFButton* TempButton;
		
		while (c < g_NumOfButtons)
		{
			TempButton = Buttons[c];
				 // FRAME для каждого BUTTON
			if ( TempButton->Frame( InputClass->DxInputStruct, ObjectActiveBUSY ))
			{
				if (LastActiveButtonIndex > -1 )
				// Вырубаем курсор в предыдущем активном Edit, если он там включён
				SwithOFFCursor( Buttons[LastActiveButtonIndex] );

				ChangeActiveEdit( c );
				LastActiveButtonIndex = c;
			}
			// Если текущий Edit активен, то рисуем в нём курсор
			// Если объект Label активен, то ждём в нажатия клавиши
			if ( TempButton->ActiveAsEdit )
				// Какой тип элемента стал активным - была нажата кнопка мыши на Edit или Label
				switch ( TempButton->ObjectType )
				{
					// Если Label, то проверяем нужно ли ожидать нажатий на любую кнопку
					case 2:
							// Если нужно ввести кнопку, то включаем ожидание нажатия на любую кнопку
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

					// Если Edit, то начинаем процесс редактирования строки привязанной к Edit
					case 3:
						{
						int MAXLength = GlobalResourceManager->GetSentenceMaxLength( TempButton->SentenceIndex );

						char* TempString = new char [MAXLength];

						strcpy_s( TempString, (size_t)MAXLength, GlobalResourceManager->GetSentenceText( TempButton->SentenceIndex ) );

						int RealLength = (int)strlen( TempString );

						// ++++++++++++++++++++++++++++++    Моргаем курсором     +++++++++++++++++++++++++++++++++++++++++
						// Если курсор ВКЛ
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
						// Если курсор ВЫКЛ
						else
						{
							if ( TempButton->CursorOn )
							{
								TempButton->CursorOn = false;
								TempString[RealLength - 1] = NULL;
								UpdateObjectText ( TempButton, TempString );
							}
						}
						// ++++++++++++++++++++++++++++++    Моргаем курсором     +++++++++++++++++++++++++++++++++++++++++

					// Если введённый символ отображаемый, то добавляем его в текст
					// Для того чтобы кнопки нужно было отпускать и вбивалось по одному символу
					if (UTF16_KeyCODE != -1 && !KeyPressed )
					{
						KeyPressed = true;
						TempKeyCode = UTF16_KeyCODE;
					}

					// Для того чтобы кнопки нужно было отпускать и вбивалось по одному символу
					if ( KeyPressed && UTF16_KeyCODE == -1 )
					{
						KeyPressed = false;

						// Считываем реальную длинну строки
						RealLength = (int)strlen( TempString );

						// Enter
						if (TempKeyCode == 13)
						{
							// Если курсор в тексте - убираем его.
							if (TempButton->CursorOn)
							{
								TempButton->CursorOn = false;
								TempString[RealLength - 1] = NULL;
							}

							TempButton->ActiveAsEdit = false;
							UpdateObjectText ( TempButton, TempString );
							break;
						}

						// Если длинна строки меньше максимальной
						if (RealLength < MAXLength - 2 || TempKeyCode == 8) // -2  потому что нужно место для символа + в конце строки ещё /0
						{

							// Если курсор в тексте - убираем его.
							if (TempButton->CursorOn)
							{
								TempButton->CursorOn = false;
								TempString[RealLength - 1] = NULL;
							}

							char Str[2];
							// Что принимает Edit зависит от настроек
							switch ( TempButton->EditType )
							{
								case ANYSYMBOL:
							
									Str[0] = UTF16toCHAR ( TempKeyCode );
									Str[1] = NULL;
									break;

								case AS_INTEGER:

									Str[0] = UTF16toINT ( TempKeyCode );
									Str[1] = NULL;

									// Проверяем что минус можно поставить только в начале строки
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
									// Проверка на единственный символ точки в троке
									if ( Str[0] == 0x2E )
										if ( PointSet( TempString ) )
										{
											Str[0] = 0x00;
											Str[1] = NULL;
										}
									// Проверяем что минус можно поставить только в начале строки
									if ( Str[0] == 0x2D )
										if ( !strlen( TempString ) == NULL )
										{
											Str[0] = 0x00;
											Str[1] = NULL;
										}


								default:;
							}

							// Если нужно удалить символ ( BackSpace нажат )
							if (Str[0] == 8)
							{
								RealLength = (int)strlen( TempString );
								// Едаляем символ только если строка содержит хоть один символ
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
	, int IndexOfButton , int FramesSpead)
{

//	Buttons[IndexOfButton]->SetIsMouseOnButtonAnimation(NumOfVerses , NumofColom , KF2DTextureFileName , 20) ;

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

		// Если к Button присоединен текст, то смотрим его длинну для создания текстовой строки
		// Которая будет рисоваться поверх кнопки
		if (TempButton->SentenceIndex > -1 )
		{
			UpdateObjectText ( TempButton, GlobalResourceManager->GetSentenceText(TempButton->SentenceIndex) );
		}

		++c;
	}
}


void MenuControrerClass::PutLabelInsideButton( int ButtonIndex )
{
	KFButton* TempButton = Buttons[ ButtonIndex ];

	if (TempButton->SentenceIndex > -1)
	{

		UpdateObjectText ( TempButton, GlobalResourceManager->GetSentenceText(TempButton->SentenceIndex));

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
	// Ё
	if ( UTF16_KeyPressed == 0x0401 )
		return -88;
	// ё
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

		int MAXLength = GlobalResourceManager->GetSentenceMaxLength( TempButton->SentenceIndex );

		char* TempString = new char[MAXLength];

		strcpy_s( TempString, (size_t)MAXLength, GlobalResourceManager->GetSentenceText( TempButton->SentenceIndex ) );

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


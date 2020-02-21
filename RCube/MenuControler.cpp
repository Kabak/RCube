#include "stdafx.h"
#include "MenuControler.h"

MenuControllerClass::MenuControllerClass(){

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
		BUSY_ObjectIndex = -1;
	   KeyDefineExpected = false;
}

HRESULT MenuControllerClass::Init(D3DGlobalContext* D3DGC,
	KFButton_Elements ButtonsData[], int NumOfButtons, // Buttons objects , Buttons amount
	ScrollBar_Elements ScrollBarData[], int NumOfScrollbars, // ScrollBars objects , ScrollBars amount
	StringsList_Elements StringsLists[], int NumOfStringsList,// StringsLists objects , StringList amount
	ColorPicker_Elements ColorPicker[], int NumOfColorPickers,// ColorPickers objects , ColorPicker amount
	KF2DTextureAnimation *_Animation,		// background animation
	XMFLOAT4 BackGroundCoord,				// x,y - Menu position  z - width , w - height
	ID3D11ShaderResourceView* BackgroundTexture, // background texture
	ResourceManager * GlobalResManager		// Engine Resource menager
	)
{
	HRESULT result = S_OK ;
	
		McD3DGC = D3DGC;
	GlobalInput = D3DGC->m_EngineInputClass;
	GlobalResourceManager = GlobalResManager;

	g_NumOfButtons = NumOfButtons ;
	// резервируем место под кнопки
	Buttons.reserve( NumOfButtons );

	g_NumOfScrollBars = NumOfScrollbars ;
	ScrollBars.reserve ( NumOfScrollbars );

	g_NumOfStringsLists = NumOfStringsList;

	g_NumOfColorPickers = NumOfColorPickers;

	Animation = _Animation;

	BackGroundCoord.x *= McD3DGC->ScreenScale.z;
	BackGroundCoord.y *= McD3DGC->ScreenScale.w;
	BackGroundCoord.z *= McD3DGC->ScreenScale.z;
	BackGroundCoord.w *= McD3DGC->ScreenScale.w;

	int TempIndex = GlobalResourceManager->Create_Flat_Obj_Buffers ( NO_CPU_ACCESS_BUFFER, 4, 6, BackgroundTexture );
	Background = new FlatObjectClass;
	Background->Init( D3DGC->ScreenWidth, D3DGC->ScreenHeight, BackGroundCoord,
		BackgroundTexture, 
		NO_FLIP,
		GlobalResourceManager->Get_Flat_ObjectBuffers_ByIndex ( TempIndex )
	);

	// Устанавливаем анимацию на фон меню
	if ( Animation )
		Animation->SetAnimeParam ( BackGroundCoord );

	// Позиция окна X , Позиция окна Y , Размер экрана ширина , Размер экрана высота 
	XMFLOAT4 ScrCoords = McD3DGC->ScreenScale;

	//	Global Menu Items Indexes by creation order
	int ItemIndex = 0;
/*
	for (int c = 0; c < NumOfButtons; ++c )
	{
		// ADD object into autoplace system
		KFButton_Elements* ButtonInitData = &ButtonsData[c];
		MenuItem* TempItem = new MenuItem;
		TempItem->Index = ItemIndex;
		TempItem->IndexInType = c;
		TempItem->Type = ButtonInitData->Type;
		TempItem->ObjX = ( int ) ButtonInitData->_ObjParam.x;
		TempItem->ObjY = ( int )ButtonInitData->_ObjParam.y;
		TempItem->ObjWidth = ( int ) ButtonInitData->_ObjParam.z;
		TempItem->ObjHeight = ( int ) ButtonInitData->_ObjParam.w;
		MenuItemsList.push_back ( TempItem );
		++ItemIndex;
	}
	for ( int c = 0; c < NumOfScrollbars; ++c )
	{
		ScrollBar_Elements* TempScrollBarData = &ScrollBarData[c];
		MenuItem* TempItem = new MenuItem;
		TempItem->Index = ItemIndex;
		TempItem->IndexInType = c;
		TempItem->Type = SCROLLBAR;
		TempItem->ObjX = ( int ) TempScrollBarData->ObjParam.x;
		TempItem->ObjY = ( int ) TempScrollBarData->ObjParam.y;
		TempItem->ObjWidth = ( int ) TempScrollBarData->ObjParam.z;
		TempItem->ObjHeight = ( int ) TempScrollBarData->ObjParam.w;
		MenuItemsList.push_back ( TempItem );
		++ItemIndex;
	}
	for ( int c = 0; c < NumOfStringsList; ++c )
	{
		StringsList_Elements* TempStringsListData = &StringsLists[c];
		MenuItem* TempItem = new MenuItem;
		TempItem->Index = ItemIndex;
		TempItem->IndexInType = c;
		TempItem->Type = STR_LIST;
		TempItem->ObjX = ( int ) TempStringsListData->ObjParam.x;
		TempItem->ObjY = ( int ) TempStringsListData->ObjParam.y;
		TempItem->ObjWidth = ( int ) TempStringsListData->ObjParam.z;
		TempItem->ObjHeight = ( int ) TempStringsListData->ObjParam.w;
		MenuItemsList.push_back ( TempItem );
		++ItemIndex;
	}
	for ( int c = 0; c < NumOfColorPickers; ++c )
	{
		ColorPicker_Elements* TempColorPickerData = &ColorPicker[c];
		MenuItem* TempItem = new MenuItem;
		TempItem->Index = ItemIndex;
		TempItem->IndexInType = c;
		TempItem->Type = COLOR_PICK;
		TempItem->ObjX = ( int ) TempColorPickerData->ObjParam.x;
		TempItem->ObjY = ( int ) TempColorPickerData->ObjParam.y;
		TempItem->ObjWidth = ( int ) TempColorPickerData->ObjParam.z;
		TempItem->ObjHeight = ( int ) TempColorPickerData->ObjParam.w;
		MenuItemsList.push_back ( TempItem );
		++ItemIndex;
	}

	// CALCULATE MENU ITEMS POS & DIM
	CalculateMenuObjectsPositions ( MenuItemsList, ItemsGroups );
	// CALCULATE MENU ITEMS POS & DIM

	size_t Size = ItemsGroups.size ();
	for ( size_t c = 0; c < Size; ++c )
	{
		size_t ItemsIndexs = ItemsGroups[c]->ItemIndexes.size ();

		GroupItem* Group = ItemsGroups[c];

		for ( size_t i = 0; i < ItemsIndexs; ++i )
		{
			MenuItem* Item = MenuItemsList[i];


			if ( Item->Type < 4 )
			{

			}
			Item->ObjX *= McD3DGC->ScreenScale.z;
			Item->ObjY *= McD3DGC->ScreenScale.w;

			Item->ObjWidth *= McD3DGC->ScreenScale.z;
			Item->ObjHeight *= McD3DGC->ScreenScale.w;

		}


	}



	// Delete All Groups
	size_t Size = ItemsGroups.size ();
	for ( size_t c = 0; c < Size; ++c )
	{
		delete ItemsGroups[c];
	}
	ItemsGroups.clear ();

	// Clear autoplce system
	int c = 0;
	Size = (int)MenuItemsList.size ();
	while ( c < Size )
	{
		delete MenuItemsList[c];
		++c;
	}
	MenuItemsList.clear ();
*/


ItemIndex = 0;
// Place Objects By scaling
for ( int c = 0; c < NumOfButtons; ++c )
{
	KFButton_Elements* ButtonInitData = &ButtonsData[c];

//	MenuItem* TempItem = MenuItemsList[ItemIndex];
	ButtonInitData->_ObjParam.x *= McD3DGC->ScreenScale.z;
	ButtonInitData->_ObjParam.y *= McD3DGC->ScreenScale.w;
	ButtonInitData->_ObjParam.z *= McD3DGC->ScreenScale.z;
	ButtonInitData->_ObjParam.w *= McD3DGC->ScreenScale.w;

//	++ItemIndex;
}

for ( int c = 0; c < NumOfScrollbars; ++c )
{
	ScrollBar_Elements* TempScrollBarData = &ScrollBarData[c];

//	MenuItem* TempItem = MenuItemsList[ItemIndex];
	TempScrollBarData->ObjParam.x *= McD3DGC->ScreenScale.z;
	TempScrollBarData->ObjParam.y *= McD3DGC->ScreenScale.w;
	TempScrollBarData->ObjParam.z *= McD3DGC->ScreenScale.z;
	TempScrollBarData->ObjParam.w *= McD3DGC->ScreenScale.w;

//	++ItemIndex;
}

for ( int c = 0; c < NumOfStringsList; ++c )
{
	StringsList_Elements* TempStringsListData = &StringsLists[c];

//	MenuItem* TempItem = new MenuItem;
	TempStringsListData->ObjParam.x *= McD3DGC->ScreenScale.z;
	TempStringsListData->ObjParam.y *= McD3DGC->ScreenScale.w;
	TempStringsListData->ObjParam.z *= McD3DGC->ScreenScale.z;
	TempStringsListData->ObjParam.w *= McD3DGC->ScreenScale.w;

//	++ItemIndex;
}

for ( int c = 0; c < NumOfColorPickers; ++c )
{
	ColorPicker_Elements* TempColorPickerData = &ColorPicker[c];

//	MenuItem* TempItem = MenuItemsList[ItemIndex];
	TempColorPickerData->ObjParam.x *= McD3DGC->ScreenScale.z;
	TempColorPickerData->ObjParam.y *= McD3DGC->ScreenScale.w;
	TempColorPickerData->ObjParam.z *= McD3DGC->ScreenScale.z;
	TempColorPickerData->ObjParam.w *= McD3DGC->ScreenScale.w;

//	++ItemIndex;
}




// Buttons initialisation
	KFButton* TempButton;
	ItemIndex = 0;	// Menu Item Index Hierarchy
	int c = 0;
	while(c < NumOfButtons)
	{

		TempButton = new KFButton ;
		Buttons.emplace_back (TempButton);
		KFButton_Elements* ButtonInitData = &ButtonsData[c];

		// Если к Button присоединен текст, то смотрим его длинну для создания текстовой строки
		// Которая будет рисоваться поверх кнопки
		int LabelSize = (int)strlen( ButtonInitData->Label );

		int SentenceIndex = -1;

		// Если создаётся объект Edit или Label, то Label можут быть NULL, но ButtonsData[c].Data->MaxLength обязательно должно быть больше 0 
		// чтобы Edit и Label корректно работал
		if (( ButtonInitData->Type == LABEL || ButtonInitData->Type == EDIT ) && 
		   (  ButtonInitData->Data != NULL ))
		{
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
			GlobalResourceManager
			);

		TempButton->SetIfButtonPressTexture( ButtonInitData->IsClickTexture);
		TempButton->SetIfMouseOnButtonTexture( ButtonInitData->IsMouseOnButtonTexture );
		TempButton->SetIfButtonNotEnalbledTexture( ButtonInitData->IsNotEnalbledTexture );
		TempButton->SetOsnTexture ( ButtonInitData->OsnTexture );
		TempButton->ObjectIndex = ItemIndex++;
		++c ;
	}

// ScrollBars initialisation
	ScrollBarClass* TempScrollBar;
    c = 0 ;
	while (c < NumOfScrollbars)
	{
		TempScrollBar = new ScrollBarClass;
		ScrollBars.emplace_back ( TempScrollBar );

		ScrollBar_Elements* TempScrollBarData = &ScrollBarData[c];

			TempScrollBar->Init(
			D3DGC,
			BackGroundCoord,
			*TempScrollBarData,
			GlobalResourceManager
			);

			TempScrollBar->SetMouseOnButtonTexture( TempScrollBarData->MouseOnButtonTexture);
			TempScrollBar->SetMouseOnTravellerTexture( TempScrollBarData->MouseOnTravellerTexture );
			TempScrollBar->SetBarNotEnalbledTexture( TempScrollBarData->BodyNotEnalbledTexture );
			TempScrollBar->SetBarNotEnalbledButtonsTexture( TempScrollBarData->ButtonNotEnalbledTexture );
			TempScrollBar->SetTravellerNotEnalbledTexture( TempScrollBarData->TravellerNotEnalbledTexture );
			TempScrollBar->SetButtonsPressTexture( TempScrollBarData->ButtonPressTexture );
			TempScrollBar->SetTravellerPressTexture( TempScrollBarData->TravellerPressTexture );
			TempScrollBar->ObjectIndex = ItemIndex++;
			++c;
	}

// StruingLists initialisation
	StringsListClass* TempStringsList;
	c = 0;
	while ( c < NumOfStringsList )
	{

		TempStringsList = new StringsListClass;
		StringsList.push_back( TempStringsList );

		StringsList_Elements* TempStringsListData = &StringsLists[c];

		TempStringsList->Init(
			D3DGC,
			BackGroundCoord,
			*TempStringsListData,
			GlobalResourceManager
		);
		TempStringsList->ObjectIndex = ItemIndex++;
		++c;
	}

// ColorPickers initialization
	ColorPickerClass* TempColorPicker;
	c = 0;
	while ( c < NumOfColorPickers )
	{

		TempColorPicker = new ColorPickerClass;
		ColorPickers.push_back ( TempColorPicker );

		ColorPicker_Elements* TempColorPickerData = &ColorPicker[c];

		TempColorPicker->Init (
			D3DGC,
			ScrCoords,
			BackGroundCoord,
			*TempColorPickerData,
			GlobalResourceManager
			);
		TempColorPicker->ObjectIndex = ItemIndex++;
		++c;
	}
	return result;
}


MenuControllerClass::~MenuControllerClass()
{
	
}

// Размещает текст поверх элементов меню в центе элемента по Y
void MenuControllerClass::UpdateObjectText ( KFButton* TempButton, char* Str )
{
	int YPos;
	// Получаем высоту текста в пикселях, чтобы можно было его выровнять по центру объекта содержащего строку
	long TextHight = ( long )GlobalResourceManager->GetSentenceHeight ( TempButton->SentenceIndex );

	if ( TempButton->ABSolute_Height > TextHight )
		YPos = ( int ) TempButton->ABSoluteY + ( TempButton->ABSolute_Height - TextHight ) / 2;// -2;
	else
		YPos = (int)TempButton->ABSoluteY + (TextHight - TempButton->ABSolute_Height) / 2 + 2;
	
	GlobalResourceManager->UpdateSentence ( TempButton->SentenceIndex, Str, int ( TempButton->ABSoluteX + 10 ), YPos );
}


HRESULT MenuControllerClass::Frame( InputClass* InputClass, int UTF16_KeyCODE, FPSTimers &fpstimers)
{

	// Все происходит при условии, что конкретное меню сейчас имеет статус IsMenuActive
	if ( IsMenuActive )
	{
		// Таймер для моргания курсора с частотой 0.5 - пол секунды
		if (fpstimers.FpsRate > 0 )
			CursorTime +=  1 / float( fpstimers.FpsRate );

		if ( CursorTime > 0.5f ) //  Частота 0.5f секунда
		{
			CursorTime = 0.0f;

			CursorOn_Off ? CursorOn_Off = false : CursorOn_Off = true;
		}

// Обработка всех кнопок на форме
		int c = 0;
		KFButton* TempButton;
		ButtonState State;

		while (c < g_NumOfButtons)
		{
			TempButton = Buttons[c];

				 // FRAME для каждого BUTTON
			if ( TempButton->Frame( InputClass->DxInputStruct, BUSY_ObjectIndex ))
			{
				if (LastActiveButtonIndex > -1 )
				// Вырубаем курсор в предыдущем активном Edit, если он там включён
				SwithOFFCursor( Buttons[LastActiveButtonIndex] );

				ChangeActiveEdit( c );
				LastActiveButtonIndex = c;
			}



/*
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// Checking if About strings should be displayed
			TempButton->GetButtonState ( State );
			if ( State.IsMouseOnButton )
				// If texture with text does not exist
				if ( TempButton->TextureIndex != -1 )
				{
				// display texture with About message
				}
				else
				{

				// Create texture , store it index , display it
				}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/



			// Если текущий Edit активен, то рисуем в нём курсор
			// Если объект Label активен, то ждём в нём нажатия клавиши
			if ( TempButton->ActiveAsEdit )
				// Какой тип элемента стал активным - была нажата кнопка мыши на Edit или Label
				switch ( TempButton->ObjectType )
				{
					// Если Label, то проверяем нужно ли ожидать нажатий на любую кнопку
					case LABEL:
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
										TempButton->EditFinished = true;
										UpdateObjectText ( TempButton,
									    GlobalInput->GetVKeyText ( TempButton->VirtualKeyIndex, TempButton->SecondSlot )
										);
									}
							}
							break;

					// Если Edit, то начинаем процесс редактирования строки привязанной к Edit
					case EDIT:
					{
						int MAXLength = GlobalResourceManager->GetSentenceMaxLength ( TempButton->SentenceIndex );

						char* TempString = new char[MAXLength];

						strcpy_s ( TempString, ( size_t ) MAXLength, GlobalResourceManager->GetSentenceText ( TempButton->SentenceIndex ) );

						int RealLength = ( int ) strlen ( TempString );

						// ++++++++++++++++++++++++++++++    Моргаем курсором     +++++++++++++++++++++++++++++++++++++++++
						// Если курсор ВКЛ
						if ( CursorOn_Off )
						{
							if ( !TempButton->CursorOn )
							{
								if ( ( RealLength + 1 ) < MAXLength )
								{
									TempButton->CursorOn = true;
									strcat_s ( TempString, MAXLength, "|" );
									UpdateObjectText ( TempButton, TempString );
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
						if ( UTF16_KeyCODE != -1 && !KeyPressed )
						{
							KeyPressed = true;
							TempKeyCode = UTF16_KeyCODE;
						}

						// Для того чтобы кнопки нужно было отпускать и вбивалось по одному символу
						if ( KeyPressed && UTF16_KeyCODE == -1 )
						{
							KeyPressed = false;

							// Считываем реальную длинну строки
							RealLength = ( int ) strlen ( TempString );

							// Enter
							if ( TempKeyCode == 13 )
							{
								// Если курсор в тексте - убираем его.
								if ( TempButton->CursorOn )
								{
									TempButton->CursorOn = false;
									TempString[RealLength - 1] = NULL;
								}

								TempButton->ActiveAsEdit = false;
								TempButton->EditFinished = true;
								UpdateObjectText ( TempButton, TempString );
								break;
							}

							// Если длинна строки меньше максимальной
							if ( RealLength < MAXLength - 2 || TempKeyCode == 8 ) // -2  потому что нужно место для символа + в конце строки ещё /0
							{

								// Если курсор в тексте - убираем его.
								if ( TempButton->CursorOn )
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
									if ( Str[0] == 0x2D )
										if ( !strlen ( TempString ) == NULL )
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
										if ( PointSet ( TempString ) )
										{
											Str[0] = 0x00;
											Str[1] = NULL;
										}
									// Проверяем что минус можно поставить только в начале строки
									if ( Str[0] == 0x2D )
										if ( !strlen ( TempString ) == NULL )
										{
											Str[0] = 0x00;
											Str[1] = NULL;
										}


									default:;
								}

								// Если нужно удалить символ ( BackSpace нажат )
								if ( Str[0] == 8 )
								{
									RealLength = ( int ) strlen ( TempString );
									// Едаляем символ только если строка содержит хоть один символ
									if ( RealLength > 0 )
									{
										RealLength = ( int ) strlen ( TempString );
										TempString[RealLength - 1] = NULL;
									}
								}
								else
								{
									strcat_s ( TempString, MAXLength, Str );
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
			ScrollBars[c]->Frame( InputClass->DxInputStruct, fpstimers, BUSY_ObjectIndex );
			++c;
		}

// StringsList
		c = 0;
		while ( c < g_NumOfStringsLists )
		{
			StringsList[c]->Frame( InputClass->DxInputStruct, fpstimers, BUSY_ObjectIndex );
			++c;
		}

// ColorPickers
		c = 0;
		while ( c < g_NumOfColorPickers )
		{
			ColorPickerClass *Picker = ColorPickers[c];

			Picker->Frame ( InputClass->DxInputStruct, fpstimers, BUSY_ObjectIndex );
			++c;
		}
	}

	if ( Animation )
		Animation->Frame( fpstimers );

	return S_OK ;
}


void MenuControllerClass::ChangeActiveEdit( int ActiveNumber )
{
	int c = 0;
	while (c < g_NumOfButtons)
	{
		if ( c != ActiveNumber )
			Buttons[c]->ActiveAsEdit = false;

		++c;
	}
}


void MenuControllerClass::SetButtonAnimation(int NumOfVerses , int NumofColom , ID3D11ShaderResourceView * KF2DTextureFileName
	, int IndexOfButton , int FramesSpead)
{

//	Buttons[IndexOfButton]->SetIsMouseOnButtonAnimation(NumOfVerses , NumofColom , KF2DTextureFileName , 20) ;

}


void MenuControllerClass::SetButtonPos( int ButtonIndex, float x, float y )
{
	KFButton* TButton;
	TButton = Buttons[ButtonIndex];
	TButton->ObjParam.x = x;
	TButton->ObjParam.y = y;
	TButton->ResetButtonParam();
//	TButton->UpdateABSElementPos();
}


void MenuControllerClass::SetButtonPos(int ButtonIndex, float Percent )
{
	KFButton* TButton;
	TButton = Buttons[ButtonIndex];
	TButton->ObjParam.x = TButton->ObjOriginalParam.x * Percent;
	TButton->ObjParam.y = TButton->ObjOriginalParam.y * Percent;
	TButton->ResetButtonParam();
//	TButton->UpdateABSElementPos();
}


void MenuControllerClass::SetButtonSize( int ButtonIndex, float width , float heigth )
{
	KFButton* TButton;
	TButton = Buttons[ButtonIndex];
	TButton->ObjParam.z = width;
	TButton->ObjParam.w = heigth;
	TButton->ResetButtonParam();
	TButton->UpdateABSElementSize();
}


void MenuControllerClass::SetButtonSize(int ButtonIndex, float Percent)
{
	KFButton* TButton;
	TButton = Buttons[ButtonIndex];
	TButton->ObjParam.z = TButton->ObjOriginalParam.z * Percent;
	TButton->ObjParam.w = TButton->ObjOriginalParam.w * Percent;
	TButton->ResetButtonParam();
	TButton->UpdateABSElementSize();
}


void MenuControllerClass::SetButtonParam(int ButtonIndex, XMFLOAT4& Param)
{
	KFButton* TButton;
	TButton = Buttons[ButtonIndex];
	TButton->ObjParam = Param;
	TButton->ResetButtonParam();
	TButton->UpdateABSElementAll();
}


void MenuControllerClass::SetButtonToOrigin( int ButtonIndex )
{
	KFButton* TButton;
	TButton = Buttons[ButtonIndex];
	TButton->ObjParam = TButton->ObjOriginalParam;
	TButton->ResetButtonParam();
	TButton->UpdateABSElementAll();
}


void MenuControllerClass::SetMenuPos(XMFLOAT2& Pos)
{
	Background->ObjParam.x = Pos.x;
	Background->ObjParam.y = Pos.y;
	Background->SetObjParam();
	UpdateMenuElementsPos();
}


void MenuControllerClass::UpdateMenuElementsPos()
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


void  MenuControllerClass::SetMenuSize( float Percent )
{
	Background->ObjParam.z = Background->ObjOriginalParam.z * Percent;
	Background->ObjParam.w = Background->ObjOriginalParam.w * Percent;
	Background->SetObjParam();
	UpdateMenuElementsAll();
}


void MenuControllerClass::UpdateMenuElementsAll()
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


void MenuControllerClass::AddAnyMenuEffect(int Index)
{


}


void MenuControllerClass::SetMenuActive(bool Value)
{

	IsMenuActive = Value;

}


void MenuControllerClass::PutLabelsInsideButtons()
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


void MenuControllerClass::PutLabelInsideButton( int ButtonIndex )
{
	KFButton* TempButton = Buttons[ ButtonIndex ];

	if (TempButton->SentenceIndex > -1)
	{

		UpdateObjectText ( TempButton, GlobalResourceManager->GetSentenceText(TempButton->SentenceIndex));

	}
}


char MenuControllerClass::UTF16toCHAR ( int UTF16_KeyPressed )
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


char MenuControllerClass::UTF16toINT ( int UTF16_KeyPressed )
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
char MenuControllerClass::UTF16toFLOAT ( int UTF16_KeyPressed )
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


bool MenuControllerClass::PointSet ( char* Str )
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


void MenuControllerClass::SwithOFFCursor ( KFButton* Button )
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


float MenuControllerClass::GetScrollBarValue( int ScrBarIndex )
{
	int Size = (int)ScrollBars.size();
	if ( ScrBarIndex < Size )
		return ScrollBars[ScrBarIndex]->GetCurrentValue();
	else
		return -666.66666666f;

}


void MenuControllerClass::SetScrollBarEnable( int ScrBarIndex, bool value )
{
	int Size = ( int ) ScrollBars.size();

	if ( ScrBarIndex < Size )
		ScrollBars[ScrBarIndex]->SetEnable( value );

}


bool MenuControllerClass::SetScrollBarValue( int ScrBarIndex, float &Value )
{
	int Size = ( int ) ScrollBars.size();

	if ( ScrBarIndex < Size )
	{
		ScrollBars[ScrBarIndex]->SetValue( Value );
		return true;
	}
		return false;
}


bool MenuControllerClass::GetButtonState( int ButtonIndex )
{
	int Size = (int)Buttons.size();

	ButtonState State;
	KFButton* Button = Buttons[ButtonIndex];

	if ( ButtonIndex < Size )
	{
		if ( Button->ObjectType == CHECKBOX )
			return Button->Checked;
		else // Other button types
		{
			Button->GetButtonState ( State );
			return State.IsPress;
		}
	}

	return false;
}


bool MenuControllerClass::GetEditFinished ( int ButtonIndex )
{
	int Size = ( int ) Buttons.size ();

	KFButton* Button = Buttons[ButtonIndex];

	if ( ButtonIndex < Size )
	{
		if ( Button->ObjectType == EDIT || Button->ObjectType == LABEL )
			return Button->EditFinished;
	}

	return false;
}


bool MenuControllerClass::GetButtonChanged ( int ButtonIndex )
{
	int Size = ( int ) Buttons.size ();

	KFButton* Button = Buttons[ButtonIndex];

	if ( ButtonIndex < Size )
	{
		if ( Button->ObjectType == CHECKBOX )
			return Button->Changed;
	}

	return false;

}


bool MenuControllerClass::GetScrollBarChanged ( int ScrBarIndex )
{ 
	int Size = ( int ) ScrollBars.size ();

	if ( ScrBarIndex < Size )
	{
		return ScrollBars[ScrBarIndex]->Changed;
	}

	return false;
}


void MenuControllerClass::ClearButtonState( int ButtonIndex )
{
	int Size = (int)Buttons.size();

	if( ButtonIndex < Size )
		Buttons[ButtonIndex]->ClearButtonState();
}


void MenuControllerClass::SetButtonEnable( int ButtonIndex, bool value )
{
	int Size = (int)Buttons.size();

	if( ButtonIndex < Size )
		Buttons[ButtonIndex]->SetEnable( value );

}


float MenuControllerClass::GetButtonTextAsFloat ( int ButtonIndex )
{
	int Size = ( int ) Buttons.size ();
	if ( ButtonIndex < Size )
	{
		KFButton* Button = Buttons[ButtonIndex];

		if ( Button->ObjectType == EDIT && ( Button->EditType == AS_FLOAT || Button->EditType == AS_INTEGER ) )
		{
			SentenceType * Sentence = GlobalResourceManager->RCube_Sentences[Button->SentenceIndex];
			return ( float ) ( atof ( Sentence->Text ) );
		}
	}

	return -50505.50505f;
}


int MenuControllerClass::GetButtonTextAsInt ( int ButtonIndex )
{
	int Size = ( int ) Buttons.size ();

	if ( ButtonIndex < Size )
	{
		KFButton* Button = Buttons[ButtonIndex];

		if ( Button->ObjectType == EDIT && Button->EditType == AS_INTEGER )
		{
			SentenceType * Sentence = GlobalResourceManager->RCube_Sentences[Button->SentenceIndex];
			return atoi ( Sentence->Text );
		}
	}
	
	return -50505050;
}


XMFLOAT4 MenuControllerClass::GetButtonColour ( int ButtonIndex )
{
	int Size = ( int ) Buttons.size ();
	if ( ButtonIndex < Size &&  Buttons[ButtonIndex]->ObjectType == COLOR_PICK )
	{
		return Buttons[ButtonIndex]->GetButtonColor ();
	}
	else
	{
		return XMFLOAT4 { -50505.50505f , -50505.50505f , -50505.50505f , -50505.50505f };
	}
}



//////////////////////////////////////////////////////////////////////////////////////////////


void MenuControllerClass::CalculateMenuObjectsPositions ( vector <MenuItem*> &MenuItemsList, vector <GroupItem*> &ItemsGroups )
{
	int ElementIndex = FindMinXYPosItem ( MenuItemsList, ItemsGroups );

	GroupItem* NewGroup = new GroupItem;
	ItemsGroups.push_back ( NewGroup );

	PushIntoGroup ( 0, ElementIndex );

	size_t ItemsAmount = MenuItemsList.size ();

	for ( size_t i = 0; i < ItemsAmount; ++i )
	{
		MenuItem* Item = MenuItemsList[i];

		size_t GroupsAmount = ItemsGroups.size ();

		for ( size_t g = 0; g < GroupsAmount; ++g )
		{
			// If Item not in group
			if ( Item->Group == -1 )
			{
				size_t AmountInGroup = ItemsGroups[g]->ItemIndexes.size ();

				for ( size_t gi = 0; gi < AmountInGroup; ++gi )
				{

					bool Direction = false;
					bool Identical = false;
					// ???????????????????????????????????????????????????????????					
					if ( Item->Group == -1 )
					{
						GroupItem* Group = ItemsGroups[g];

						int ItemIndex = Group->ItemIndexes[gi];

						CompareItems ( Item, MenuItemsList[ItemIndex], Direction, Identical );
						// Item seems should be added into existed group
						if ( Identical )
						{
							Group->Direction = Direction;
							PushIntoGroup ( ( int ) g, Item->Index );
							break;
						}
					}
				}
			}
		}
		// If Item did not fit any existed groups
		if ( Item->Group == -1 )
		{
			GroupItem* NewGroup = new GroupItem;

			size_t NewGroupIndex = ItemsGroups.size ();
			ItemsGroups.push_back ( NewGroup );
			PushIntoGroup ( ( int ) NewGroupIndex, Item->Index );
		}
	}
}


void MenuControllerClass::PushIntoGroup ( int GroupIndex, int MenuItemIndex )
{
	GroupItem* Group = ItemsGroups[GroupIndex];

	int First = (int)Group->ItemIndexes.size ();

	MenuItem* TempItem = MenuItemsList[MenuItemIndex];

	// Push first element
	if ( First == 0 )
	{
		// Updating group dimentions
		Group->GroupX = TempItem->ObjX;
		Group->GroupY = TempItem->ObjY;
		Group->GroupWidth = TempItem->ObjWidth;
		Group->GroupHeight = TempItem->ObjHeight;
		Group->Direction = false;		// Assuming group spread Y direction
		Group->ItemIndexes.push_back ( MenuItemIndex );	// Store first element into group
	
		TempItem->Group = GroupIndex;// Assign group index to Item 
	}
	// Not First Item in the group
	else
	{
		int LastGroupItemIndex = ( int ) Group->ItemIndexes.size ();

		int ItemIndex = Group->ItemIndexes[LastGroupItemIndex - 1];

		MenuItem*  LastGroupItem = MenuItemsList[ItemIndex];
		
		if ( Group->Direction == _X_ )
		{
			TempItem->PreviousItemDistance = TempItem->ObjX - ( LastGroupItem->ObjX + LastGroupItem->ObjWidth );
		}
		else
		{
			TempItem->PreviousItemDistance = TempItem->ObjY - ( LastGroupItem->ObjY + LastGroupItem->ObjHeight );
		}

		Group->ItemIndexes.push_back ( MenuItemIndex );	// Store first element into group

		TempItem->Group = GroupIndex;

		UpdateGroupDim ( GroupIndex, MenuItemIndex );
	}

}


void MenuControllerClass::UpdateGroupDim ( int GroupIndex, int NewItemIndex )
{
	GroupItem* Group = ItemsGroups[GroupIndex];
	MenuItem* TempItem = MenuItemsList[NewItemIndex];

	if ( TempItem->ObjX > Group->GroupWidth )
	{
		Group->Direction = _X_;
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!  DistanceBetweenElements  should be other
		Group->DistanceBetweenElements = TempItem->ObjX - Group->GroupWidth;
		Group->GroupWidth = Group->GroupWidth + Group->DistanceBetweenElements + TempItem->ObjWidth;
	}
	else if ( TempItem->ObjY > Group->GroupHeight )
	{
		Group->Direction = _Y_;
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!  DistanceBetweenElements  should be other
		Group->DistanceBetweenElements = TempItem->ObjY - Group->GroupHeight;
		Group->GroupHeight = Group->GroupHeight + Group->DistanceBetweenElements + TempItem->ObjHeight;
	}
}


bool MenuControllerClass::FindItemBetweenItems ( GroupItem* Group, MenuItem* CheckItem, bool &Direction )
{
	bool result = false;

	size_t Size = MenuItemsList.size ();
	for ( size_t c = 0; c < Size; ++c )
	{
		MenuItem* Item = MenuItemsList[c];

		// Do not check with themselves - exclude One & Two and Items in other groups
		if ( Item->Index != CheckItem->Index )
		{
			// Checking according to Direction item placed  X same line or Y
			// X
			if ( Direction )
			{
				// Check Item & Group neighborhood from any side
				if ( Group->GroupX + Group->GroupWidth > CheckItem->ObjX )
				{
					// Item between Group - CheckItem ( left side near the group )
					if ( Item->ObjX + Item->ObjWidth < Group->GroupX  && Item->ObjX > CheckItem->ObjX + CheckItem->ObjWidth )
						return true;
				}
				else
				{
					// Item between Group - CheckItem ( right side near the group )
					if ( Item->ObjX + Item->ObjWidth < CheckItem->ObjX  && Item->ObjX > Group->GroupX + Group->GroupWidth )
						return true;
				}
			}
			else
			{
				// If One plased near ZERO menu point
				if ( Group->GroupY + Group->GroupHeight > CheckItem->ObjY )
				{
					// Item between Two - One
					if ( Item->ObjY + Item->ObjHeight < Group->GroupY  && Item->ObjY > CheckItem->ObjY + CheckItem->ObjHeight )
						return true;
				}
				else
				{
					// Item between Two - One
					if ( Item->ObjY + Item->ObjHeight < CheckItem->ObjY  && Item->ObjY > Group->GroupY + Group->GroupHeight )
						return true;
				}
			}
		}
	}

	return result;
}


void MenuControllerClass::CompareItems ( MenuItem* One, MenuItem* GroupedItem, bool &Direction, bool &Identical )
{
	bool Yes = false;
	if ( One->ObjWidth == GroupedItem->ObjWidth && One->ObjHeight == GroupedItem->ObjHeight )
	{
		 if ( One->ObjX == GroupedItem->ObjX )
		 {
			 Direction = _Y_;
			 Yes = true;
		 }
		 else if ( One->ObjY == GroupedItem->ObjY )
		 {
			 Direction = _X_;
			 Yes = true;
		 }

		 if (Yes)
			 if ( !FindItemBetweenItems ( ItemsGroups[GroupedItem->Group], One, Direction ) )
			 {
				 Identical = true;
			 }
	}
}


int MenuControllerClass::FindMinXYPosItem ( vector <MenuItem*> &MenuItemsList, vector <GroupItem*> &ItemsGroups )
{
	int Index = 0;
	int X = 10000;
	int Y = 10000;

	size_t Size = MenuItemsList.size ();
	for ( size_t c = 0; c < Size; ++c )
	{
		MenuItem* Item = MenuItemsList[c];
		
		if ( Item->ObjX < X && Item->ObjY < Y )
		{
			X = Item->ObjX;
			Y = Item->ObjY;
			Index = (int)c;
		}
	}
	return Index;
}



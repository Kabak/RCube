#include "stdafx.h"
#include "ScrollBarClass.h"

ScrollBarClass::ScrollBarClass() {

//	memset( this, 0, sizeof( ScrollBarClass ) );

			 ButtonsTexture = nullptr;
	   MouseOnButtonTexture = nullptr;
		 ButtonPressTexture = nullptr;
   NotEnalbledButtonTexture = nullptr;
	MouseOnTravellerTexture = nullptr;
		   TravellerTexture = nullptr;
	  TravellerPressTexture = nullptr;
NotEnalbledTravellerTexture = nullptr;
				BodyTexture = nullptr;
	 NotEnalbledBodyTexture = nullptr;
				  MinButton = nullptr;
				  MaxButton = nullptr;
				  Traveller = nullptr;
					   Body = nullptr;
			  ButtonPressed = false;
		   TravellerPressed = false;
				 UpSideDown = false;

				  TimerStop = false;
					   Time = 0.0f;


}


HRESULT ScrollBarClass::Init( 
	D3DGlobalContext* D3DGC,
	XMFLOAT4& _ScreenCoords,
	XMFLOAT4& _FormCoord,
	KFScrollBar_Elements& TempScrollBarData,
	Flat_Obj_Buffers* _BuffersMin,
	Flat_Obj_Buffers* _BuffersMax,
	Flat_Obj_Buffers* _BuffersTraveler,
	Flat_Obj_Buffers* _Body
	) 
{
	HRESULT Result = S_OK;

//	GlobalResourceManager = GlobalResManager;
	Horizontal = TempScrollBarData.Horizontal;

//	StaticTravelerWidth = _StaticTravelerWidth;

	MinValue = TempScrollBarData.Values.x;
	MaxValue = TempScrollBarData.Values.y;
	CurrentValue = TempScrollBarData.Values.z;
	StepValue = TempScrollBarData.Values.w;

	ScreenCoords = _ScreenCoords;
	FormCoord = _FormCoord;
	UpSideDown = TempScrollBarData.UpSideDown;

	Enabled = true;

	ObjParam = TempScrollBarData.ObjParam;

	// специальные переменные скролл бара
	MinMixButtonsShow = TempScrollBarData.ShowButtons;
	// специальные переменные скролл бара

	// текстуры
	ButtonsTexture = TempScrollBarData.ButtonsTexture;
	TravellerTexture = TempScrollBarData.TravellerTexture;
	BodyTexture = TempScrollBarData.BodyTexture;
	// текстуры
	
	// определение размеров текстуры
	if ( TempScrollBarData.OsnTextureResource )
	{
		D3D11_TEXTURE2D_DESC* InputTextureDesc = new D3D11_TEXTURE2D_DESC;
		ID3D11Texture2D* InputTexture;
		TempScrollBarData.OsnTextureResource->QueryInterface<ID3D11Texture2D>(&InputTexture);
		InputTexture->GetDesc(InputTextureDesc);

		if( Horizontal )
		{
			if (ObjParam.z == 0.0f)
				ObjParam.z = (float)InputTextureDesc->Width;
			if (ObjParam.w == 0.0f)
				ObjParam.w = (float)InputTextureDesc->Height;
		}
		else
		{
			if( ObjParam.z == 0.0f )
				ObjParam.z = (float)InputTextureDesc->Height;
			if( ObjParam.w == 0.0f )
				ObjParam.w = (float)InputTextureDesc->Width;
		
		}
		delete InputTextureDesc;
		InputTexture->Release();
	}
	else
	{
		ObjParam.z = 0.0f;
		ObjParam.w = 0.0f;
	}

	ObjOriginalParam = ObjParam;

	// Поиск середины ScrollBar для размещения кнопок Min и Max
	if ( Horizontal )
		MiddleBodyPos = ObjParam.w / 2;
	else
		MiddleBodyPos = ObjParam.z / 2;

	// определение размеров текстуры
	_2DPixelXmax = float(_ScreenCoords.z / 2);
	_2DPixelYmax = float(_ScreenCoords.w / 2);

	MinButton = new FlatObjectClass;
	MaxButton = new FlatObjectClass;
	Traveller = new FlatObjectClass;
		 Body = new FlatObjectClass;
		// Временная переменная для размещения кнопок и ползунка вокруг тела
		XMFLOAT4 _MinButton = { 0.0f, 0.0f, 0.0f, 0.0f };
		XMFLOAT4 _MaxButton = { 0.0f, 0.0f, 0.0f, 0.0f };
		XMFLOAT4 _TravellerButton = { 0.0f, 0.0f, 0.0f, 0.0f };

		// Если текстура для кнопок : меньше, больше задана, и TempData z,w = 0.0f , то берём размеры текстуры
		if ( ButtonsTexture )
		{
			GetTextureParam( ButtonsTexture, _MinButton );
		}

		if ( TravellerTexture )
		{
			GetTextureParam( TravellerTexture, _TravellerButton );
		}

		SetButtonsToBody( Horizontal, _MinButton, _MaxButton, _TravellerButton );



		if ( Horizontal )
		{
			// тело
			Body->Init ( D3DGC->ScreenWidth, D3DGC->ScreenHeight, ObjParam, BodyTexture, NO_FLIP, _Body );
			// Инициализируем MinButton
			MinButton->Init( D3DGC->ScreenWidth, D3DGC->ScreenHeight, _MinButton, ButtonsTexture, NO_FLIP, _BuffersMin );
			// Инициализируем MaxButton
			MaxButton->Init( D3DGC->ScreenWidth, D3DGC->ScreenHeight, _MaxButton, ButtonsTexture, FLIP_HORIZONTAL, _BuffersMax );
			// Инициализируем TravellerButton	
			Traveller->Init( D3DGC->ScreenWidth, D3DGC->ScreenHeight, _TravellerButton, TravellerTexture, NO_FLIP, _BuffersTraveler );
		}
		else
		{
			// тело
			Body->Init ( D3DGC->ScreenWidth, D3DGC->ScreenHeight, ObjParam, BodyTexture, ROTATE90, _Body );
			// Инициализируем MinButton
			MinButton->Init( D3DGC->ScreenWidth, D3DGC->ScreenHeight, _MinButton, ButtonsTexture, ROTATE90, _BuffersMin );
			// Инициализируем MaxButton
			MaxButton->Init( D3DGC->ScreenWidth, D3DGC->ScreenHeight, _MaxButton, ButtonsTexture, FLIP_HORIZONTAL_ROTATE_90, _BuffersMax );
			// Инициализируем TravellerButton	
			Traveller->Init( D3DGC->ScreenWidth, D3DGC->ScreenHeight, _TravellerButton, TravellerTexture, ROTATE90, _BuffersTraveler );
		}
		
		// Инвертируем значение для правильной инициализации, скролбара вверх ногами + - вверху   - - внизу
		if ( UpSideDown )
		{
			float val = UpSideDownValue ();
			ChangeTravelerPositionByValue ( val );
			CurrentValue = val;
		}
		// + внизу   - вверху
		else
			ChangeTravelerPositionByValue( CurrentValue );

		UpdateABSElementAll();

		SetScrollBarBodyParam();

	BodyVertexes = new Vertex_FlatObject[4];

	BodyVertexes[0].Position = XMFLOAT3( left, top, 0.0f );  // Top left.
	BodyVertexes[1].Position = XMFLOAT3( right, bottom, 0.0f );  // Bottom right.
	BodyVertexes[2].Position = XMFLOAT3( left, bottom, 0.0f );  // Bottom left.
	BodyVertexes[3].Position = XMFLOAT3( right, top, 0.0f );  // Top right.

	//если горизонтально
	if (Horizontal) 
	{
		BodyVertexes[0].TexCoord = XMFLOAT2( 0.0f, 0.0f );
		BodyVertexes[1].TexCoord = XMFLOAT2( 1.0f, 1.0f );
		BodyVertexes[2].TexCoord = XMFLOAT2( 0.0f, 1.0f );
		BodyVertexes[3].TexCoord = XMFLOAT2( 1.0f, 0.0f );
	}
	//если вертикально
	else
	{
		BodyVertexes[0].TexCoord = XMFLOAT2( 0.0f, 1.0f );
		BodyVertexes[1].TexCoord = XMFLOAT2( 1.0f, 0.0f );
		BodyVertexes[2].TexCoord = XMFLOAT2( 1.0f, 1.0f );
		BodyVertexes[3].TexCoord = XMFLOAT2( 0.0f, 0.0f );
	}

//	unsigned int indices[6] = { 0, 1, 2, 0, 3, 1 };

	Body->Buffers->FlatObjectVB->Update ( BodyVertexes );
	Body->Buffers->IndexBs->Update ( (Index_Type*)FlatObjectIndices );
	Body->Buffers->RenderTexture = BodyTexture;

	delete[] BodyVertexes;

	return Result;
}


ScrollBarClass::~ScrollBarClass() 
{
	RCUBE_DELETE ( MinButton ); 
	RCUBE_DELETE ( MaxButton );
	RCUBE_DELETE ( Traveller );
	RCUBE_DELETE ( Body );
}

void ScrollBarClass::SetMouseOnButtonTexture(ID3D11ShaderResourceView * Texture) {
	MouseOnButtonTexture = Texture;

}

void ScrollBarClass::SetButtonsPressTexture(ID3D11ShaderResourceView * Texture) {

	ButtonPressTexture = Texture;

}

void ScrollBarClass::SetBarNotEnalbledButtonsTexture(ID3D11ShaderResourceView * Texture) {

	NotEnalbledButtonTexture = Texture;

}

void ScrollBarClass::SetMouseOnTravellerTexture(ID3D11ShaderResourceView * Texture) {

	MouseOnTravellerTexture = Texture;

}

void ScrollBarClass::SetTravellerPressTexture(ID3D11ShaderResourceView * Texture) {

	TravellerPressTexture = Texture;

}

void ScrollBarClass::SetTravellerNotEnalbledTexture(ID3D11ShaderResourceView * Texture) {

	NotEnalbledTravellerTexture = Texture;

}

void ScrollBarClass::SetBarNotEnalbledTexture(ID3D11ShaderResourceView * Texture) {

	NotEnalbledBodyTexture = Texture;

}


// Получить высоту и ширину текстуры для объекта - кнопки в ScrollBar
// Если текстура для кнопок : меньше, больше задана, и TempData z,w = 0.0f , то берём размеры текстуры
void ScrollBarClass::GetTextureParam( ID3D11ShaderResourceView * Texture, XMFLOAT4& Dimention )
{
	D3D11_TEXTURE2D_DESC* InputTextureDesc = new D3D11_TEXTURE2D_DESC;
	ID3D11Texture2D* InputTexture;
	ID3D11Resource* MyRes;

	Texture->GetResource( &MyRes );
	MyRes->QueryInterface<ID3D11Texture2D>( &InputTexture );
	InputTexture->GetDesc( InputTextureDesc );
	
	if( Horizontal )
	{
		if( Dimention.z == 0.0f )
			Dimention.z = (float)InputTextureDesc->Width;
		if( Dimention.w == 0.0f )
			Dimention.w = (float)InputTextureDesc->Height;
	}
	else
	{
		if( Dimention.z == 0.0f )
			Dimention.z = (float)InputTextureDesc->Height;
		if( Dimention.w == 0.0f )
			Dimention.w = (float)InputTextureDesc->Width;
	}
	delete InputTextureDesc;
	InputTexture->Release();
	MyRes->Release();
}


float ScrollBarClass::UpSideDownValue ()
{
	float UpSideDownVal = 0.0f;
	float		TempMax = 0.0f;
	float		TempMin = 0.0f;
	float	TempCurrent = 0.0f;
	float		 Middle = 0.0f;

	if ( MinValue < 0.0f )
	{
		TempMax = MaxValue + abs ( MinValue );
		Middle = TempMax / 2;
	}

	else if ( MinValue == 0.0f )
	{
		Middle = MaxValue / 2;
	}

	else if ( MinValue > 0.0f )
	{
		TempMax = MaxValue - MinValue;
		Middle = TempMax / 2;
	}
	
	// Получаем середину шкалы для нашего диапозона
	float MiddleforRealScales = MinValue + Middle;

	if ( CurrentValue < MiddleforRealScales )
	{
		float HowFarFromMiddle = MiddleforRealScales - CurrentValue;
		UpSideDownVal = MiddleforRealScales + HowFarFromMiddle;
	}

	if ( CurrentValue == MiddleforRealScales )
	{
		UpSideDownVal = MiddleforRealScales;
	}

	if ( CurrentValue > MiddleforRealScales )
	{
		float HowFarFromMiddle = CurrentValue - MiddleforRealScales;
		UpSideDownVal = MiddleforRealScales - HowFarFromMiddle;
	}
		
		return UpSideDownVal;
}


void ScrollBarClass::ChangeTravelerPositionByScroll( long XX )
{
	float LengthInPixels = 0.0f;
	float ValuePlus = 0.0f;

	// Вычисляем общий диапозон значений
	float Diapozon = ( MaxValue - MinValue );

	if ( Horizontal )
	{
		// Сколько длинна пространства для ползунка в пикселях
		LengthInPixels = ObjParam.z - MIN_TRAVELER_SIZE;
	}
	else
	{
		LengthInPixels = ObjParam.w - MIN_TRAVELER_SIZE;
	}
	// Сколько в одном пикселе - шаг ползунка за один пиксель сдвига ползунка на экране
	float OneStep = Diapozon / LengthInPixels;
	// Находим смещение ползунка относительно начала
	ValuePlus = XX * OneStep;

	CurrentValue += ValuePlus;
	
	CurrentValue > MaxValue ? CurrentValue = MaxValue : CurrentValue;
	CurrentValue < MinValue ? CurrentValue = MinValue : CurrentValue;

	ChangeTravelerPositionByValue( CurrentValue );

}


void ScrollBarClass::SetValue( float value )
{
	value > MaxValue ? CurrentValue = MaxValue : CurrentValue = value;
	value < MinValue ? CurrentValue = MinValue : CurrentValue = value;

	ChangeTravelerPositionByValue( CurrentValue );

}


void ScrollBarClass::ChangeTravelerPositionByValue( float &value )
{
	float TravellerSize = MIN_TRAVELER_SIZE;
	float LengthInPixels = 0.0f;
	float OnePixelStep = 0.0f;
//	long StepInPixels = 0.0f;
	float CurPos = 0.0f;
	float Diapozon = 0.0f;

	value > MaxValue ? value = MaxValue : value;
	value < MinValue ? value = MinValue : value;
	
	// Вычисляем общий диапозон значений
	Diapozon = GetValuesDiaposone();

//	float Ratio = abs ( Diapozon / StepValue );

	if ( Horizontal )
	{
		// Сколько длинна пространства для ползунка в пикселях
		LengthInPixels = ObjParam.z - MIN_TRAVELER_SIZE;
		
		if( Diapozon < LengthInPixels )
		{
			TravellerSize = ObjParam.z - Diapozon;
			Traveller->ObjParam.z = TravellerSize;

			LengthInPixels = ObjParam.z - TravellerSize;

		}
		else
		{


		}
	}
	else
	{
		// Сколько длинна пространства для ползунка в пикселях
		LengthInPixels = ObjParam.w - MIN_TRAVELER_SIZE;

		if( Diapozon < LengthInPixels )
		{
			TravellerSize = ObjParam.w - Diapozon;
			Traveller->ObjParam.w = TravellerSize;

			LengthInPixels = ObjParam.w - TravellerSize;

		}

	}

	// Сколько в одном пикселе - шаг ползунка за один пиксель сдвига ползунка на экране
	OnePixelStep = Diapozon / LengthInPixels;

	float temp = ( value - MinValue ) / StepValue;

	CurPos = ( temp / Diapozon ) * LengthInPixels;
	
	// Если ScrollBox горизонтальный
	if (Horizontal)
	{
		Traveller->ObjParam.x = FormCoord.x + ObjParam.x + CurPos;
	}
	// Если ScrollBox вертикальный
	else
	{
		Traveller->ObjParam.y = FormCoord.y + ObjParam.y + CurPos;
	}

	UpdateTravelerAll();
	Traveller->SetObjParam();

}


void ScrollBarClass::BodyClickToValue( POINT &Position )
{
	long XX = 0;

	if ( Horizontal)
	{
		float MiddleTravellerPos = Traveller->ObjParam.x + Traveller->ObjParam.z - Traveller->ObjParam.z / 2;

		XX = Position.x - (long)MiddleTravellerPos;
		
	}
	else
	{
		float MiddleTravellerPos = Traveller->ObjParam.y + Traveller->ObjParam.w - Traveller->ObjParam.w / 2;

		XX = Position.y - (long)MiddleTravellerPos;
	}

	ChangeTravelerPositionByScroll ( XX );
}


int ScrollBarClass::Frame( DXINPUTSTRUCT& InputClass, FPSTimers& fpstimers, bool &ObjectBUSY )
{
	// Проверка, была ли нажата мышка и не отпущена именно на этом объекте
	// Если не была нажата вовсе или нажата на этом объекте, то выполняем обработку этого объекта
	if( ObjectBUSY && !ButtonPressed && !TravellerPressed && !BodyPressed )
		return 0;

	if ( Enabled )
	{
		// Таймер для прокрутки скролбокса чпустя 0.5f после нажатия на кнупку
		if ( ButtonPressed && !TimerStop )
		{
			if ( fpstimers.FpsRate > 0 )
			Time += 1 / float( fpstimers.FpsRate );

			if ( Time > 0.5f ) //  Частота 0.5f секунда
			{
				Time = 0.0f;
				TimerStop = true;
			}
		}

		// Это нужно чтобы при нажатии на ползунке и удерживании нажатой мышки ползунок следовал за мышкой даже,
		// если курсор мышки ушёл с тела ScrollBox
		if ( InputClass.m_mouseState.rgbButtons[0] && TravellerPressed )
		{
			if ( TravellerPreseed( InputClass ))
				ObjectBUSY = true;
			else
				if ( !ButtonPressed && !BodyPressed )
					ObjectBUSY = false;

			return 0;
		}
		else
		{
			TravellerPressed = false;
//			ButtonPressed = false;
			BodyPressed = false;
			ObjectBUSY = false;
		}

		// Мышка на ползунке ?
		if (( InputClass.MousePos.x > TravelABSoluteX && InputClass.MousePos.x < TravelABSoluteX + TravelABSolute_Width ) &&
			( InputClass.MousePos.y > TravelABSoluteY && InputClass.MousePos.y < TravelABSoluteY + TravelABSolute_Height ))
		{
			if ( TravellerPreseed( InputClass ) )
				ObjectBUSY = true;
			else
				ObjectBUSY = false;
				
		}
		// Мышка НЕ на ползунке
		else
		{
			// сбрасываем текстуру ползунка по умолчанию
			Traveller->SetObjectTexture ( TravellerTexture );

			// Пpоверяем попала ли мышка в Body ( ТЕЛО не кнопки )
			if (( ( InputClass.MousePos.x > ABSoluteX && InputClass.MousePos.x < ABSoluteX + ABSolute_Width ) &&
				( InputClass.MousePos.y > ABSoluteY && InputClass.MousePos.y < ABSoluteY + ABSolute_Height ) ))
			{
				// Нажата на теле
				if (InputClass.m_mouseState.rgbButtons[ 0 ])  // Мышка была нажата на объекте
				{
					BodyClickToValue( InputClass.MousePos );
					BodyPressed = true;
					ObjectBUSY = true;
				}
				else
				{
					BodyPressed = false;
					ObjectBUSY = false;
				}

			}
		}

		// Пpоверяем попала ли мышка в MinButton
		if( MinMixButtonsShow )
		if (((InputClass.MousePos.x > MinABSoluteX && InputClass.MousePos.x < MinABSoluteX + MinABSolute_Width) &&
			(InputClass.MousePos.y > MinABSoluteY && InputClass.MousePos.y < MinABSoluteY + MinABSolute_Height)))
		{
			if (InputClass.m_mouseState.rgbButtons[0] && TravellerPressed == false )  // Мышка была нажата на объекте
			{
				MinButton->SetObjectTexture( ButtonPressTexture );

				if ( !ButtonPressed || TimerStop )
				{
					CurrentValue -= StepValue;
					ChangeTravelerPositionByValue( CurrentValue );
					ButtonPressed = true;
					ObjectBUSY = true;
				}
			}
			else
			{
				if ( MouseOnButtonTexture )
					MinButton->SetObjectTexture( MouseOnButtonTexture );
				else
					MinButton->SetObjectTexture( ButtonsTexture );
				
				ButtonPressed = false;
				ObjectBUSY = false;
				TimerStop = false;
				Time = 0.0f;
			}
		}
		else
		{
			MinButton->SetObjectTexture( ButtonsTexture );
		}

		// Пpоверяем попала ли мышка в MaxButton
		if ( MinMixButtonsShow )
		if (( ( InputClass.MousePos.x > MaxABSoluteX && InputClass.MousePos.x < MaxABSoluteX + MaxABSolute_Width ) &&
			( InputClass.MousePos.y > MaxABSoluteY && InputClass.MousePos.y < MaxABSoluteY + MaxABSolute_Height ) ))
		{
			if ( InputClass.m_mouseState.rgbButtons[0] && TravellerPressed == false )  // Мышка была нажата на объекте
			{
				MaxButton->SetObjectTexture( ButtonPressTexture );
				if( !ButtonPressed || TimerStop )
				{
					CurrentValue += StepValue;
					ChangeTravelerPositionByValue( CurrentValue );
					ButtonPressed = true;
					ObjectBUSY = true;
				}
			}
			else
			{
				if (MouseOnButtonTexture)
					MaxButton->SetObjectTexture( MouseOnButtonTexture );
				else
					MaxButton->SetObjectTexture( ButtonsTexture );
				
				ButtonPressed = false;
				ObjectBUSY = false;
				TimerStop = false;
				Time = 0.0f;
			}
		}
		else
		{
			MaxButton->SetObjectTexture( ButtonsTexture );
		}

	}

	return 0;
}


void ScrollBarClass::SetScrollBarBodyParam()
{
		left = -_2DPixelXmax + ( FormCoord.x + ObjParam.x );
		right = left + ObjParam.z;
		top = _2DPixelYmax - ( FormCoord.y + ObjParam.y );
		bottom = top - ObjParam.w;
}


void ScrollBarClass::UpdateABSElementSize()
{
	ABSolute_Width = (long)ObjParam.z;
	ABSolute_Height = (long)ObjParam.w;
}


void ScrollBarClass::UpdateABSElementAll()
{
	ABSoluteX = long( FormCoord.x + ObjParam.x );
	ABSoluteY = long( FormCoord.y + ObjParam.y );
	ABSolute_Width = (long)ObjParam.z;
	ABSolute_Height = (long)ObjParam.w;

	MinABSoluteX = long( MinButton->ObjParam.x );
	MinABSoluteY = long( MinButton->ObjParam.y );
	MinABSolute_Width = (long)MinButton->ObjParam.z;
	MinABSolute_Height = (long)MinButton->ObjParam.w;

	MaxABSoluteX = long( MaxButton->ObjParam.x );
	MaxABSoluteY = long( MaxButton->ObjParam.y );
	MaxABSolute_Width = (long)MaxButton->ObjParam.z;
	MaxABSolute_Height = (long)MaxButton->ObjParam.w;

	TravelABSoluteX = long( Traveller->ObjParam.x );
	TravelABSoluteY = long( Traveller->ObjParam.y );
	TravelABSolute_Width = (long)Traveller->ObjParam.z;
	TravelABSolute_Height = (long)Traveller->ObjParam.w;
}


void ScrollBarClass::UpdateABSElementPos()
{
	ABSoluteX = long( FormCoord.x + ObjParam.x );
	ABSoluteY = long( FormCoord.y + ObjParam.y );

	MinABSoluteX = long( MinButton->ObjParam.x );
	MinABSoluteY = long( MinButton->ObjParam.y );

	MaxABSoluteX = long( MaxButton->ObjParam.x );
	MaxABSoluteY = long( MaxButton->ObjParam.y );

	TravelABSoluteX = long( Traveller->ObjParam.x );
	TravelABSoluteY = long( Traveller->ObjParam.y );
}


void ScrollBarClass::UpdateTravelerPos()
{
	TravelABSoluteX = long( Traveller->ObjParam.x );
	TravelABSoluteY = long( Traveller->ObjParam.y );
}


void ScrollBarClass::UpdateTravelerAll()
{
	TravelABSoluteX = long( Traveller->ObjParam.x );
	TravelABSoluteY = long( Traveller->ObjParam.y );
	TravelABSolute_Width = ( long ) Traveller->ObjParam.z;
	TravelABSolute_Height = ( long ) Traveller->ObjParam.w;
}


// Садим кнопки посередине тела ScrollBox
void ScrollBarClass::SetButtonsToBody( bool Horizontal, XMFLOAT4 &MinButton, XMFLOAT4 &MaxButton, XMFLOAT4 &TravellerButton )
{
	if ( Horizontal )
	{
		MinButton.x = FormCoord.x + ObjParam.x - MinButton.z;
		// Помещаем Min кнопку посередине тела ScrollBox
		MinButton.y = FormCoord.y + ObjParam.y + MiddleBodyPos - ObjParam.w / 2;

		MaxButton.x = FormCoord.x + ObjParam.x + ObjParam.z;
		// Помещаем Max кнопку посередине тела ScrollBox
		MaxButton.y = FormCoord.y + ObjParam.y + MiddleBodyPos - ObjParam.w / 2;
		MaxButton.z = MinButton.z;
		MaxButton.w = MinButton.w = ObjParam.w;

		TravellerButton.x = FormCoord.x + ObjParam.x;
		// Помещаем ползунок кнопку посередине тела ScrollBox
		TravellerButton.y = FormCoord.y + ObjParam.y + MiddleBodyPos / 2;
		TravellerButton.z = MIN_TRAVELER_SIZE;
		TravellerButton.w = ObjParam.w / 2; //

	}
	else
	{
		MinButton.x = FormCoord.x + ObjParam.x + MiddleBodyPos - ObjParam.z / 2;
		MinButton.y = FormCoord.y + ObjParam.y - MinButton.w;
		MaxButton.x = FormCoord.x + ObjParam.x + MiddleBodyPos - ObjParam.z / 2;
		MaxButton.y = FormCoord.y + ObjParam.y + ObjParam.w;
		MaxButton.z = MinButton.z = ObjParam.z;
		MaxButton.w = MinButton.w;

		TravellerButton.x = FormCoord.x + ObjParam.x + MiddleBodyPos / 2; 
		// Помещаем ползунок кнопку посередине тела ScrollBox
		TravellerButton.y = FormCoord.y + ObjParam.y + MiddleBodyPos;
		TravellerButton.w = MIN_TRAVELER_SIZE;
		TravellerButton.z = ObjParam.z / 2; //
	}
}


float ScrollBarClass::GetCurrentValue()
{
	if( UpSideDown )
		return UpSideDownValue ();
	else
		return CurrentValue;
}

bool ScrollBarClass::CheckSign( float Val )
{
 if ( Val < 0 )
	return false;

	return true;
}


float ScrollBarClass::GetValuesDiaposone()
{
	float Diapozon = 0.0f;

	bool Max = CheckSign( MaxValue );
	bool Min = CheckSign( MinValue );
	
	// Если оба крайних значения в ScrollBar отрицательные
	if ( Max == false && Min == false )
	{
		// Вычисляем общий диапозон значений
		Diapozon = abs( MinValue - MaxValue ) / StepValue;
	}
	else
	{
		// Вычисляем общий диапозон значений
		Diapozon = ( MaxValue - MinValue ) / StepValue;
	}

	return Diapozon;
}


bool ScrollBarClass::TravellerPreseed( DXINPUTSTRUCT& InputClass )
{
	// Нажата на ползунке
	if( InputClass.m_mouseState.rgbButtons[0] )  // Мышка была нажата на объекте
	{
		// Фиксируем положение мышки во время нажатия
		if( !TravellerPressed )
			MousePosXX = InputClass.MousePos;

		TravellerPressed = true;

		if( TravellerPressTexture )
		{
			Traveller->SetObjectTexture ( TravellerPressTexture );

			if( Horizontal )
			{
				if( MousePosXX.x != InputClass.MousePos.x )
				{
					ChangeTravelerPositionByScroll( InputClass.MousePos.x - MousePosXX.x );
					MousePosXX = InputClass.MousePos;
				}
			}
			else
			{
				if( MousePosXX.y != InputClass.MousePos.y )
				{
					ChangeTravelerPositionByScroll( InputClass.MousePos.y - MousePosXX.y );
					MousePosXX = InputClass.MousePos;
				}
			}


		}
		else
		{
			Traveller->SetObjectTexture ( TravellerTexture );
		}
	}
	else
	{
		TravellerPressed = false;

		if( MouseOnTravellerTexture )
			Traveller->SetObjectTexture ( MouseOnTravellerTexture );
		else
			Traveller->SetObjectTexture ( TravellerTexture );
	}

	return TravellerPressed;
}


void ScrollBarClass::SetEnable ( bool Value )
{
	if ( Value )
	{
		Enabled = true;
		MinButton->SetObjectTexture( ButtonsTexture );
		MaxButton->SetObjectTexture( ButtonsTexture );
		Traveller->SetObjectTexture ( TravellerTexture );
		Buffers->RenderTexture = BodyTexture;
	}
	else
	{
		Enabled = false;
		if ( NotEnalbledButtonTexture )
		{
			MinButton->SetObjectTexture( NotEnalbledButtonTexture );
			MaxButton->SetObjectTexture( NotEnalbledButtonTexture );
		}
		else
		{
			MinButton->SetObjectTexture( ButtonsTexture );
			MaxButton->SetObjectTexture( ButtonsTexture );
		}

		if ( NotEnalbledTravellerTexture )
			Traveller->SetObjectTexture ( NotEnalbledTravellerTexture );
		else
			Traveller->SetObjectTexture ( TravellerTexture );
		
		if (NotEnalbledBodyTexture)
		{
			Buffers->RenderTexture = NotEnalbledBodyTexture;
		}

		else
		{
			Buffers->RenderTexture = BodyTexture;
		}

	}

}
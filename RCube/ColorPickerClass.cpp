#include "stdafx.h"
#include "ColorPickerClass.h"
#include "ResourceManager.h"

ColorPickerClass::ColorPickerClass ()
{
	Panthon			= nullptr;
	ColorPicker		= nullptr;
	AlphaPicker		= nullptr;

	PanthonTexture	= nullptr;

	Dummy = { 0.0f, 0.0f, 0.0f, 0.0f };
}


ColorPickerClass::~ColorPickerClass ()
{
	ResManager->Delete_Flat_ObjectBuffers ( AlphaPickerBufferIndex );
	ResManager->Delete_Flat_ObjectBuffers ( ColorPickerBufferIndex );
	ResManager->Delete_Flat_ObjectBuffers ( PanthonBufferIndex );
	RCUBE_DELETE ( Panthon );
	RCUBE_DELETE ( ColorPicker );
	RCUBE_DELETE ( AlphaPicker );
}


HRESULT ColorPickerClass::Init ( D3DGlobalContext* D3DGC,
								 XMFLOAT4& _ScreenCoords,
								 XMFLOAT4& _FormCoord,
								 ColorPicker_Elements& TempColorPickerData,
								 ResourceManager * _GlobalResourceManager
								)
{
	HRESULT Result = S_OK;

	Local_D3DGC = D3DGC;

	ResManager = _GlobalResourceManager;

//	Style = TempColorPickerData.Style;

	Horizontal = TempColorPickerData.Horizontal;

	Panthon_Alpha_Thickness = TempColorPickerData._Panthon_Alpha_Thickness;

	ScreenCoords = _ScreenCoords;
	FormCoord = _FormCoord;
	
	Enabled = true;

	ObjParam = TempColorPickerData.ObjParam;

	CheckingColorPickerParam ();	// Setting the default param if initial user settings wrong

	PanthonTexture = TempColorPickerData.PanthonTexture;

	// Apply initial color
	CurrentColor.Fl4 = TempColorPickerData.ActiveColorValue;
	RGBAtoCOLORREF ( CurrentColor.Fl4, PanthonSelectedColor );	// Setting Panthon 
	RGBAtoCOLORREF ( CurrentColor.Fl4, CurrentColorCOLORREF );	// Setting CurrentColor
	RGBAtoCOLORREF ( CurrentColor.Fl4, ColorPickerColorCOLORREF );// Setting ColorPicker


	// Save default demention & position   
	ObjOriginalParam = ObjParam;

	Panthon		= new FlatObjectClass;
	ColorPicker = new FlatObjectClass;
	AlphaPicker = new FlatObjectClass;

	XMFLOAT4 ColorPickerBody = { 0.0f, 0.0f, 0.0f, 0.0f };
	XMFLOAT4 PanthonBody	 = { 0.0f, 0.0f, 0.0f, 0.0f };
	XMFLOAT4 AlphaBody		 = { 0.0f, 0.0f, 0.0f, 0.0f };


	// Set Selector on ColorPicker body
	ConstructBody ( Horizontal, ColorPickerBody, PanthonBody, AlphaBody, Panthon_Alpha_Thickness );

	PanthonBufferIndex = ResManager->Create_Flat_Obj_Buffers ( NO_CPU_ACCESS_BUFFER, 4, 6, nullptr );
	ColorPickerBufferIndex = ResManager->Create_Flat_Obj_Buffers ( NO_CPU_ACCESS_BUFFER, 4, 6, nullptr );
	AlphaPickerBufferIndex = ResManager->Create_Flat_Obj_Buffers ( NO_CPU_ACCESS_BUFFER, 4, 6, nullptr );

	ColorPicker->Init ( D3DGC->ScreenWidth, D3DGC->ScreenHeight, ColorPickerBody, D3DGC->BackBuffer_ProxyTextureSRV, NO_FLIP, ResManager->Get_Flat_ObjectBuffers_ByIndex ( ColorPickerBufferIndex ) );

	// Initialize ColorSelector elevents buffers
	if ( Horizontal )
	{
		Panthon->Init ( D3DGC->ScreenWidth, D3DGC->ScreenHeight, PanthonBody, PanthonTexture, ROTATE90, ResManager->Get_Flat_ObjectBuffers_ByIndex ( PanthonBufferIndex ) );
		AlphaPicker->Init ( D3DGC->ScreenWidth, D3DGC->ScreenHeight, AlphaBody, D3DGC->BackBuffer_ProxyTextureSRV, ROTATE90, ResManager->Get_Flat_ObjectBuffers_ByIndex ( AlphaPickerBufferIndex ) );
	}
	else
	{
		Panthon->Init ( D3DGC->ScreenWidth, D3DGC->ScreenHeight, PanthonBody, PanthonTexture, NO_FLIP, ResManager->Get_Flat_ObjectBuffers_ByIndex ( PanthonBufferIndex ) );
		AlphaPicker->Init ( D3DGC->ScreenWidth, D3DGC->ScreenHeight, AlphaBody, D3DGC->BackBuffer_ProxyTextureSRV, NO_FLIP, ResManager->Get_Flat_ObjectBuffers_ByIndex ( AlphaPickerBufferIndex ) );
	}

	Panthon->UpdateVertexPos ();
	GeneratePanthonVertexes ();
	
	ColorPicker->UpdateVertexPos ();
	GenerateColorPickerVertexes ();

	AlphaPicker->UpdateVertexPos ();
	GenerateAlphaPickerVertexes ();

	// Set ColorSelector position on menu surface
	UpdateABSElementAll ();

	return Result;
}


int ColorPickerClass::Frame ( DXINPUTSTRUCT& InputClass, FPSTimers& fpstimers, bool &ObjectBUSY )
{
	// Проверка, была ли нажата мышка и не отпущена именно на этом объекте
	// Если не была нажата вовсе или нажата на этом объекте, то выполняем обработку этого объекта
	if ( ObjectBUSY && !PanthonPressed && !ColorPickerPressed && !AlphaPickerPressed )
		return 0;

	if ( Enabled )
	{
		// If mouse was pressed on Panthon boby
		if ( ( ( InputClass.MousePos.x > PanthonABSoluteX && InputClass.MousePos.x < PanthonABSoluteX + PanthonABSolute_Width ) &&
			( InputClass.MousePos.y > PanthonABSoluteY && InputClass.MousePos.y < PanthonABSoluteY + PanthonABSolute_Height ) ) )
		{
			// Нажата на теле
			if ( InputClass.m_mouseState.rgbButtons[0] )  // Мышка была нажата на объекте
			{
				PanthonClickToValue ( InputClass.MousePos );
				PanthonPressed = true;
				ObjectBUSY = true;
			}
			else
			{
				PanthonPressed = false;
				ObjectBUSY = false;
			}

		}
		// If mouse with pressed button moved outside ColorPicker body
		else if ( PanthonPressed == true )
		{
			PanthonPressed = false;
			ObjectBUSY = false;
		}

		//  If mouse was pressed on ColorPicker boby
		if ( ( ( InputClass.MousePos.x > ColorPickerABSoluteX && InputClass.MousePos.x < ColorPickerABSoluteX + ColorPickerABSolute_Width ) &&
			( InputClass.MousePos.y > ColorPickerABSoluteY && InputClass.MousePos.y < ColorPickerABSoluteY + ColorPickerABSolute_Height ) ) )
		{
			// Нажата на теле
			if ( InputClass.m_mouseState.rgbButtons[0] )  // Мышка была нажата на объекте
			{
				ColorPickerClickToValue ( InputClass.MousePos );
				ColorPickerPressed = true;
				ObjectBUSY = true;
			}
			else
			{
				ColorPickerPressed = false;
				ObjectBUSY = false;
			}
		}
		else if ( ColorPickerPressed == true )
		{
			ColorPickerPressed = false;
			ObjectBUSY = false;
		}
		//  If mouse was pressed on AlphaPicker boby
		if ( ( ( InputClass.MousePos.x > AlphaPickerABSoluteX && InputClass.MousePos.x < AlphaPickerABSoluteX + AlphaPickerABSolute_Width ) &&
			( InputClass.MousePos.y > AlphaPickerABSoluteY && InputClass.MousePos.y < AlphaPickerABSoluteY + AlphaPickerABSolute_Height ) ) )
		{
			// Нажата на теле
			if ( InputClass.m_mouseState.rgbButtons[0] )  // Мышка была нажата на объекте
			{
				AlphaPickerClickToValue ( InputClass.MousePos );
				AlphaPickerPressed = true;
				ObjectBUSY = true;
			}
			else
			{
				AlphaPickerPressed = false;
				ObjectBUSY = false;
			}
		}
		else if ( AlphaPickerPressed == true )
		{
			AlphaPickerPressed = false;
			ObjectBUSY = false;
		}

	}
	//	! Enabled
	else
	{

	}

	return 0;
}


void ColorPickerClass::UpdateABSElementAll ()
{
	PanthonABSoluteX = ( long ) Panthon->ObjParam.x;
	PanthonABSoluteY = ( long ) Panthon->ObjParam.y;
	PanthonABSolute_Width = ( long ) Panthon->ObjParam.z;
	PanthonABSolute_Height = ( long ) Panthon->ObjParam.w;

	ColorPickerABSoluteX = ( long ) ColorPicker->ObjParam.x;		// Абсолютная экранная позиция по X
	ColorPickerABSoluteY = ( long ) ColorPicker->ObjParam.y;		// Абсолютная экранная позиция по Y
	ColorPickerABSolute_Width = ( long ) ColorPicker->ObjParam.z;	// Абсолютная ширина
	ColorPickerABSolute_Height = ( long ) ColorPicker->ObjParam.w;	// Абсолютная высота

	AlphaPickerABSoluteX = ( long ) AlphaPicker->ObjParam.x;		// Абсолютная экранная позиция по X
	AlphaPickerABSoluteY = ( long ) AlphaPicker->ObjParam.y;		// Абсолютная экранная позиция по Y
	AlphaPickerABSolute_Width = ( long ) AlphaPicker->ObjParam.z;	// Абсолютная ширина
	AlphaPickerABSolute_Height = ( long ) AlphaPicker->ObjParam.w;	// Абсолютная высота
}


void ColorPickerClass::PanthonClickToValue ( POINT &Position )
{
	PanthonLastClickPosition = Position;

	SetColorPicker ( ResManager->GetScreenPixelColor ( Position ) );

//	MoveSelector ( Position );

}


void ColorPickerClass::ColorPickerClickToValue ( POINT &Position )
{
	ColorPickerLastClickPosition = Position;

	SetAlphaPicker ( ResManager->GetScreenPixelColor ( Position ) );
}


void ColorPickerClass::AlphaPickerClickToValue ( POINT &Position )
{
	AlphaPickerLastClickPosition = Position;

	ApplyAlpha ( ResManager->GetScreenPixelColor ( Position ) );
}

void ColorPickerClass::RGBAtoCOLORREF ( XMFLOAT4 &Color, COLORREF &Output )
{
	RCube_VecFloat234 OutColor;
	unsigned int num4 = ( UINT ) (Color.w * 255.0f) << 24;

	OutColor.Fl4 = Color;

	OutColor.Vec *= 255.0f;

	Output = RGB ( OutColor.Fl3.x, OutColor.Fl3.y, OutColor.Fl3.z );
	Output = Output | num4;
}


void ColorPickerClass::COLORREFtoRGBA ( COLORREF Color, XMFLOAT4&RGBA )
{
	UINT R;
	UINT G;
	UINT B;
	UINT A;

	RCube_VecFloat234 OutColor;

	R =  Color & 0x000000FF;
	G = (Color & 0x0000FF00) >> 8;
	B = (Color & 0x00FF0000) >> 16;
	A = (Color & 0xFF000000) >> 24;

	OutColor.Fl4 = { float ( R ), float ( G ), float ( B ), float ( A ) };

	OutColor.Vec /= 255.0f; // Normalize colour

	RGBA = OutColor.Fl4;
}


void ColorPickerClass::SetColorPicker ( XMFLOAT4 Color )
{
	CurrentColor.Fl4 = Color;
	RGBAtoCOLORREF ( Color, ColorPickerColorCOLORREF );

	CurrentColorCOLORREF = ColorPickerColorCOLORREF;

	GenerateColorPickerVertexes ();
	GenerateAlphaPickerVertexes ();
}


void ColorPickerClass::SetColorPicker ( COLORREF Color )
{
	ColorPickerColorCOLORREF = CurrentColorCOLORREF = Color;
	COLORREFtoRGBA  ( ColorPickerColorCOLORREF, CurrentColor.Fl4 );
	GenerateColorPickerVertexes ();
	GenerateAlphaPickerVertexes ();
}


void ColorPickerClass::SetAlphaPicker ( COLORREF Color )
{
	ColorPickerColorCOLORREF = CurrentColorCOLORREF = Color;
	COLORREFtoRGBA ( ColorPickerColorCOLORREF, CurrentColor.Fl4 );
	GenerateAlphaPickerVertexes ();
}


void ColorPickerClass::SetAlphaPicker ( XMFLOAT4 Color )
{
	CurrentColor.Fl4 = Color;
	RGBAtoCOLORREF ( Color, ColorPickerColorCOLORREF );

	CurrentColorCOLORREF = ColorPickerColorCOLORREF;

	GenerateAlphaPickerVertexes ();
}


void ColorPickerClass::ApplyAlpha ( COLORREF Color )
{
	COLORREF InColor = Color;

	XMFLOAT4 InputColor;

	COLORREFtoRGBA ( InColor, InputColor );

	CurrentColor.Fl4.w = InputColor.w;

	RGBAtoCOLORREF ( CurrentColor.Fl4, CurrentColorCOLORREF );

}

XMFLOAT4 ColorPickerClass::GetSelectedColorFL4 ()
{
	return CurrentColor.Fl4;
}


COLORREF ColorPickerClass::GetSelectedColorCOLREF ()
{
	return CurrentColorCOLORREF;
}


void  ColorPickerClass::GeneratePanthonVertexes ()
{
	// Update vertex buffer to new position and set texture rotation if needed
	Vertex_FlatObject *BodyVertexes = new Vertex_FlatObject[4];
	if ( Horizontal )
	{
		Panthon->GenerateVertexes ( BodyVertexes, ROTATE90, Dummy );
	}
	else
	{
		Panthon->GenerateVertexes ( BodyVertexes, NO_FLIP, Dummy );
	}

	Panthon->Buffers->FlatObjectVB->Update ( BodyVertexes );

	delete [] BodyVertexes;
}


void  ColorPickerClass::GenerateColorPickerVertexes ()
{
	// Update vertex buffer to new position and set texture rotation if needed
	Vertex_FlatObject *BodyVertexes = new Vertex_FlatObject[4];

			RCube_VecFloat234 Color;
			Color.Vec = CurrentColor.Vec;// / 255.0f;	// Normalize color
			ColorPicker->GenerateVertexes ( BodyVertexes, COLOR_PICKER, Color.Fl4 );

			ColorPicker->Buffers->FlatObjectVB->Update ( BodyVertexes );

	delete [] BodyVertexes;
}


void ColorPickerClass::GenerateAlphaPickerVertexes ()
{
	// Update vertex buffer to new position and set texture rotation if needed
	Vertex_FlatObject *BodyVertexes = new Vertex_FlatObject[4];
	if ( Horizontal )
	{
		RCube_VecFloat234 Color;
		Color.Vec = CurrentColor.Vec;// / 255.0f;	// Normalize color
		AlphaPicker->GenerateVertexes ( BodyVertexes, ALPHA_PICKER_HORIZONTAL, Color.Fl4 );
	}
	else
	{
		RCube_VecFloat234 Color;
		Color.Vec = CurrentColor.Vec;// / 255.0f;	// Normalize color
		AlphaPicker->GenerateVertexes ( BodyVertexes, ALPHA_PICKER_VERTICAL, Color.Fl4 );
	}

	AlphaPicker->Buffers->FlatObjectVB->Update ( BodyVertexes );

	delete [] BodyVertexes;


}


void ColorPickerClass::ConstructBody ( bool _Horizontal, XMFLOAT4 &ColorPickerBody, XMFLOAT4 &PanthonBody, XMFLOAT4 &AlphaBody, float Panthon_Alpha_Thickness )
{
	Horizontal = _Horizontal;

	if ( Horizontal )
	{
		ColorPickerBody.x = FormCoord.x + ObjParam.x;
		ColorPickerBody.y = FormCoord.y + ObjParam.y;
		ColorPickerBody.z = ObjParam.z;
		ColorPickerBody.w = ObjParam.w - ( MIN_DISTANCE_FROM_BODY * 2 + Panthon_Alpha_Thickness * 2 );

		PanthonBody.x = FormCoord.x + ObjParam.x;
		PanthonBody.y = FormCoord.y + ObjParam.y + ColorPickerBody.w + MIN_DISTANCE_FROM_BODY; // distance
		PanthonBody.z = ObjParam.z;
		PanthonBody.w = Panthon_Alpha_Thickness;

		AlphaBody.x = FormCoord.x + ObjParam.x;
		AlphaBody.y = PanthonBody.y + ( MIN_DISTANCE_FROM_BODY + Panthon_Alpha_Thickness );
		AlphaBody.z = ObjParam.z;
		AlphaBody.w = Panthon_Alpha_Thickness;
	}
	else
	{
		ColorPickerBody.x = FormCoord.x + ObjParam.x + ( MIN_DISTANCE_FROM_BODY + Panthon_Alpha_Thickness );
		ColorPickerBody.y = FormCoord.y + ObjParam.y;
		ColorPickerBody.z = ObjParam.z - ( MIN_DISTANCE_FROM_BODY * 2 + Panthon_Alpha_Thickness * 2 );
		ColorPickerBody.w = ObjParam.w;

		PanthonBody.x = FormCoord.x + ObjParam.x;
		PanthonBody.y = FormCoord.y + ObjParam.y;
		PanthonBody.z = Panthon_Alpha_Thickness;
		PanthonBody.w = ObjParam.w;

		AlphaBody.x = FormCoord.x + ObjParam.x + MIN_DISTANCE_FROM_BODY * 2 + Panthon_Alpha_Thickness + ColorPickerBody.z;
		AlphaBody.y = FormCoord.y + ObjParam.y;
		AlphaBody.z = Panthon_Alpha_Thickness;
		AlphaBody.w = ObjParam.w;
	}

}


void ColorPickerClass::CheckingColorPickerParam ()
{
	ObjParam.z < MIN_COLORPICKER_SIZE ? ObjParam.z = MIN_COLORPICKER_SIZE : ObjParam.z;
	ObjParam.w < MIN_COLORPICKER_SIZE ? ObjParam.w = MIN_COLORPICKER_SIZE : ObjParam.w;

	Panthon_Alpha_Thickness < MIN_PANTHON_SIZE ? Panthon_Alpha_Thickness = MIN_PANTHON_SIZE : Panthon_Alpha_Thickness;
	Panthon_Alpha_Thickness > MAX_PANTHON_SIZE ? Panthon_Alpha_Thickness = MAX_PANTHON_SIZE : Panthon_Alpha_Thickness;
}
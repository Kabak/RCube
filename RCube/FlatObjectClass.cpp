#include "stdafx.h"
#include "FlatObjectClass.h"


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ класс для рисования (Прямоугольных) обьектов 

FlatObjectClass::FlatObjectClass()
{

	  Buffers = nullptr;
	  ObjOriginalParam = ObjParam = { 0.0f, 0.0f, 0.0f, 0.0f };
	  previousPosX = 0;
	  previousPosY = 0;
	  Dummy = { 0.0f, 0.0f, 0.0f, 0.0f };
}


HRESULT FlatObjectClass::Init(int ScreenWidth, int ScreenHeight,
	XMFLOAT4& _ObjParam, 
	ID3D11ShaderResourceView * TexCoord,
	int _TexCoordFlip,
	Flat_Obj_Buffers* _Buffers
	){

	HRESULT Result = S_OK ;
	
	Buffers = _Buffers;

	ObjOriginalParam = ObjParam = _ObjParam;

	TextureFlip = _TexCoordFlip;

	_2DPixelXmax = float( ScreenWidth / 2);
	_2DPixelYmax = float( ScreenHeight / 2);

	UpdateVertexPos();
/*
	// определение размеров текстуры
	D3D11_TexCoord2D_DESC* InputTexCoordDesc = new D3D11_TexCoord2D_DESC;
	ID3D11Resource* MyRes;
	ID3D11TexCoord2D* InputTexCoord;
	teture->GetResource(&MyRes);
	MyRes->QueryInterface<ID3D11TexCoord2D>(&InputTexCoord);
	InputTexCoord->GetDesc(InputTexCoordDesc);
	float TexCoordWidth = (float)InputTexCoordDesc->Width;
	float TexCoordHeigth = (float)InputTexCoordDesc->Height;
	delete InputTexCoordDesc;
	MyRes->Release();
	InputTexCoord->Release();
	// определение размеров текстуры
*/
		
	Vertex_FlatObject* vertexes ;

	vertexes = new Vertex_FlatObject[4];

	GenerateVertexes( vertexes, TextureFlip, Dummy );
	
	Buffers->FlatObjectVB->Update ( vertexes );
	Buffers->IndexBs->Update( (Index_Type*)FlatObjectIndices );
	Buffers->RenderTexture = TexCoord;

	delete [] vertexes ;

	return Result;
}



FlatObjectClass::~FlatObjectClass()
{

}


void FlatObjectClass::SetObjectTexture (ID3D11ShaderResourceView* TexCoord)
{
	Buffers->RenderTexture = TexCoord;
}


//  XMFLOAT4&   -     _Top , _Left , _Bottom , _Right
// Используется в классе анимации текстур
void FlatObjectClass::ResetObjectTexcord( XMFLOAT4& Position )
{
	Vertex_FlatObject* vertexes ;
	// Create the vertex array.
	vertexes = new Vertex_FlatObject[4];

	vertexes[0].Position = XMFLOAT3( left, top, 0.0f );  // Top left.
	vertexes[1].Position = XMFLOAT3( right, bottom, 0.0f );  // Bottom right.
	vertexes[2].Position = XMFLOAT3( left, bottom, 0.0f );  // Bottom left.
	vertexes[3].Position = XMFLOAT3( right, top, 0.0f );  // Top right.

	switch (TextureFlip)
	{
		// NO_FLIP
	case NO_FLIP:
		vertexes[0].TexCoord = XMFLOAT4( Position.y, Position.x, 0.0f , 0.0f );
		vertexes[1].TexCoord = XMFLOAT4( Position.w, Position.z, 0.0f, 0.0f );
		vertexes[2].TexCoord = XMFLOAT4( Position.y, Position.z, 0.0f, 0.0f );
		vertexes[3].TexCoord = XMFLOAT4( Position.w, Position.x, 0.0f, 0.0f );
		break;
		// FLIP_HORIZONTAL
	case FLIP_HORIZONTAL:
		vertexes[0].TexCoord = XMFLOAT4( 1.0f, 0.0f, 0.0f, 0.0f );
		vertexes[1].TexCoord = XMFLOAT4( 0.0f, 1.0f, 0.0f, 0.0f );
		vertexes[2].TexCoord = XMFLOAT4( 1.0f, 1.0f, 0.0f, 0.0f );
		vertexes[3].TexCoord = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		break;
		// FLIP_VERTICAL
	case FLIP_VERTICAL:
		vertexes[0].TexCoord = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		vertexes[1].TexCoord = XMFLOAT4( 1.0f, 1.0f, 0.0f, 0.0f );
		vertexes[2].TexCoord = XMFLOAT4( 0.0f, 1.0f, 0.0f, 0.0f );
		vertexes[3].TexCoord = XMFLOAT4( 1.0f, 0.0f, 0.0f, 0.0f );
		break;
		// ROTATE90
	case ROTATE90:
		vertexes[0].TexCoord = XMFLOAT4( 0.0f, 1.0f, 0.0f, 0.0f );
		vertexes[1].TexCoord = XMFLOAT4( 1.0f, 0.0f, 0.0f, 0.0f );
		vertexes[2].TexCoord = XMFLOAT4( 1.0f, 1.0f, 0.0f, 0.0f );
		vertexes[3].TexCoord = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		break;
		// ROTATE_90
	case ROTATE_90:
		vertexes[0].TexCoord = XMFLOAT4( 1.0f, 0.0f, 0.0f, 0.0f );
		vertexes[1].TexCoord = XMFLOAT4( 0.0f, 1.0f, 0.0f, 0.0f );
		vertexes[2].TexCoord = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		vertexes[3].TexCoord = XMFLOAT4( 1.0f, 1.0f, 0.0f, 0.0f );
		break;
		// FLIP_HORIZONTAL_ROTATE_90
	case FLIP_HORIZONTAL_ROTATE_90:
		vertexes[0].TexCoord = XMFLOAT4( 1.0f, 1.0f, 0.0f, 0.0f );
		vertexes[1].TexCoord = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		vertexes[2].TexCoord = XMFLOAT4( 0.0f, 1.0f, 0.0f, 0.0f );
		vertexes[3].TexCoord = XMFLOAT4( 1.0f, 0.0f, 0.0f, 0.0f );
		break;
		// ANIMATION_TEXTURE
	case ANIMATION_TEXTURE:
		vertexes[0].TexCoord = XMFLOAT4 ( Position.y, Position.x, 0.0f, -1.0f );
		vertexes[1].TexCoord = XMFLOAT4 ( Position.w, Position.z, 0.0f, -1.0f );
		vertexes[2].TexCoord = XMFLOAT4 ( Position.y, Position.z, 0.0f, -1.0f );
		vertexes[3].TexCoord = XMFLOAT4 ( Position.w, Position.x, 0.0f, -1.0f );
		break;

	default:;
	}

	Buffers->FlatObjectVB->Update ( vertexes );
	delete [] vertexes ;
}

ID3D11ShaderResourceView * FlatObjectClass::GetTexture(){

	return Buffers->RenderTexture;
}


void FlatObjectClass::SetObjParam(){

	UpdateVertexPos();

	Vertex_FlatObject* vertexes ;

	vertexes = new Vertex_FlatObject[4];

	GenerateVertexes( vertexes, TextureFlip, Dummy );

	Buffers->FlatObjectVB->Update ( vertexes );
	delete [] vertexes ;

}

// Пересчёт позиций вертексов в реальные 2D координаты экрана
void FlatObjectClass::UpdateVertexPos()
{
	left = -_2DPixelXmax + ObjParam.x;
	right = left + ObjParam.z;
	top = _2DPixelYmax - ObjParam.y;
	bottom = top - ObjParam.w;
}


void FlatObjectClass::ResizeObject(float Percent)
{
	ObjParam.z = ObjOriginalParam.z * (Percent / 100);
	ObjParam.w = ObjOriginalParam.w * (Percent / 100);
	SetObjParam();
}


void FlatObjectClass::SetOriginalSize(void)
{
	ObjParam.z = ObjOriginalParam.z;
	ObjParam.w = ObjOriginalParam.w;
}


// Генерация вертексов и текстуры объекта в соответствии с поворотом текстуры 
// Для кнопок ScrollBar, чтобы можно было использовать одну текстуру для Min и Max кнопки
void FlatObjectClass::GenerateVertexes( Vertex_FlatObject* vertexes, int TexCoordFlip, XMFLOAT4& Color )
{
	vertexes[0].Position = XMFLOAT3( left, top, 0.0f );  // Top left.
	vertexes[1].Position = XMFLOAT3( right, bottom, 0.0f );  // Bottom right.
	vertexes[2].Position = XMFLOAT3( left, bottom, 0.0f );  // Bottom left.
	vertexes[3].Position = XMFLOAT3( right, top, 0.0f );  // Top right.

	switch (TexCoordFlip)
	{
		// NO_FLIP
	case NO_FLIP:
		vertexes[0].TexCoord = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		vertexes[1].TexCoord = XMFLOAT4( 1.0f, 1.0f, 0.0f, 0.0f );
		vertexes[2].TexCoord = XMFLOAT4( 0.0f, 1.0f, 0.0f, 0.0f );
		vertexes[3].TexCoord = XMFLOAT4( 1.0f, 0.0f, 0.0f, 0.0f );
		break;
		// FLIP_HORIZONTAL
	case FLIP_HORIZONTAL:
		vertexes[0].TexCoord = XMFLOAT4( 1.0f, 0.0f, 0.0f, 0.0f );
		vertexes[1].TexCoord = XMFLOAT4( 0.0f, 1.0f, 0.0f, 0.0f );
		vertexes[2].TexCoord = XMFLOAT4( 1.0f, 1.0f, 0.0f, 0.0f );
		vertexes[3].TexCoord = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		break;
		// FLIP_VERTICAL
	case FLIP_VERTICAL:
		vertexes[0].TexCoord = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		vertexes[1].TexCoord = XMFLOAT4( 1.0f, 1.0f, 0.0f, 0.0f );
		vertexes[2].TexCoord = XMFLOAT4( 0.0f, 1.0f, 0.0f, 0.0f );
		vertexes[3].TexCoord = XMFLOAT4( 1.0f, 0.0f, 0.0f, 0.0f );
		break;
		// ROTATE90
	case ROTATE90:
		vertexes[0].TexCoord = XMFLOAT4( 0.0f, 1.0f, 0.0f, 0.0f );
		vertexes[1].TexCoord = XMFLOAT4( 1.0f, 0.0f, 0.0f, 0.0f );
		vertexes[2].TexCoord = XMFLOAT4( 1.0f, 1.0f, 0.0f, 0.0f );
		vertexes[3].TexCoord = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		break;
		// ROTATE_90
	case ROTATE_90:
		vertexes[0].TexCoord = XMFLOAT4( 1.0f, 0.0f, 0.0f, 0.0f );
		vertexes[1].TexCoord = XMFLOAT4( 0.0f, 1.0f, 0.0f, 0.0f );
		vertexes[2].TexCoord = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		vertexes[3].TexCoord = XMFLOAT4( 1.0f, 1.0f, 0.0f, 0.0f );
		break;
		// FLIP_HORIZONTAL_ROTATE_90
	case FLIP_HORIZONTAL_ROTATE_90:
		vertexes[0].TexCoord = XMFLOAT4( 1.0f, 1.0f, 0.0f, 0.0f );
		vertexes[1].TexCoord = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		vertexes[2].TexCoord = XMFLOAT4( 0.0f, 1.0f, 0.0f, 0.0f );
		vertexes[3].TexCoord = XMFLOAT4( 1.0f, 0.0f, 0.0f, 0.0f );
		break;
		// ANIMATION_TEXTURE
	case ANIMATION_TEXTURE:
		vertexes[0].TexCoord = XMFLOAT4 ( 0.0f, 0.0f, 0.0f, -1.0f );
		vertexes[1].TexCoord = XMFLOAT4 ( 1.0f, 1.0f, 0.0f, -1.0f );
		vertexes[2].TexCoord = XMFLOAT4 ( 0.0f, 1.0f, 0.0f, -1.0f );
		vertexes[3].TexCoord = XMFLOAT4 ( 1.0f, 0.0f, 0.0f, -1.0f );
		break;
		// COLOR_BUTTON
	case COLOR_BUTTON:
		vertexes[0].TexCoord = XMFLOAT4 ( 0.0f, 0.0f, 0.0f, 1.0f ); //w = 1.0f - ColorPicker draw in shader as Color, not texture
		vertexes[1].TexCoord = XMFLOAT4 ( 1.0f, 1.0f, 0.0f, 1.0f );	//w = 1.0f - ColorPicker draw in shader as Color, not texture
		vertexes[2].TexCoord = XMFLOAT4 ( 0.0f, 1.0f, 0.0f, 1.0f );	//w = 1.0f - ColorPicker draw in shader as Color, not texture
		vertexes[3].TexCoord = XMFLOAT4 ( 0.0f, 0.0f, 0.0f, 1.0f );	//w = 1.0f - ColorPicker draw in shader as Color, not texture

		vertexes[0].ColorPicker = Color;
		vertexes[1].ColorPicker = Color;
		vertexes[2].ColorPicker = Color;
		vertexes[3].ColorPicker = Color;
		break;
		// COLOR_PICKER
	case COLOR_PICKER:
		vertexes[0].TexCoord = XMFLOAT4 ( 0.0f, 0.0f, 0.0f, 1.0f ); //w = 1.0f - ColorPicker draw in shader as ColorPicker, not texture
		vertexes[1].TexCoord = XMFLOAT4 ( 1.0f, 1.0f, 0.0f, 1.0f );	//w = 1.0f - ColorPicker draw in shader as ColorPicker, not texture
		vertexes[2].TexCoord = XMFLOAT4 ( 0.0f, 1.0f, 0.0f, 1.0f );	//w = 1.0f - ColorPicker draw in shader as ColorPicker, not texture
		vertexes[3].TexCoord = XMFLOAT4 ( 0.0f, 0.0f, 0.0f, 1.0f );	//w = 1.0f - ColorPicker draw in shader as ColorPicker, not texture

		vertexes[0].ColorPicker = Color;
		vertexes[1].ColorPicker = XMFLOAT4 ( 0.0f, 0.0f, 0.0f, 1.0f );
		vertexes[2].ColorPicker = XMFLOAT4 ( 0.0f, 0.0f, 0.0f, 1.0f );
		vertexes[3].ColorPicker = XMFLOAT4 ( 1.0f, 1.0f, 1.0f, 1.0f );
		break;
		// ALPHA_PICKER_HORIZONTAL
	case ALPHA_PICKER_HORIZONTAL:
		vertexes[0].TexCoord = XMFLOAT4 ( 0.0f, 0.0f, 0.0f, 1.0f ); //w = 1.0f - ColorPicker draw in shader as ColorPicker, not texture
		vertexes[1].TexCoord = XMFLOAT4 ( 1.0f, 1.0f, 0.0f, 1.0f );	//w = 1.0f - ColorPicker draw in shader as ColorPicker, not texture
		vertexes[2].TexCoord = XMFLOAT4 ( 0.0f, 1.0f, 0.0f, 1.0f );	//w = 1.0f - ColorPicker draw in shader as ColorPicker, not texture
		vertexes[3].TexCoord = XMFLOAT4 ( 0.0f, 0.0f, 0.0f, 1.0f );	//w = 1.0f - ColorPicker draw in shader as ColorPicker, not texture
	
		vertexes[0].ColorPicker = Color;								// Top left.
		vertexes[1].ColorPicker = XMFLOAT4 ( 0.0f, 0.0f, 0.0f, 0.0f );	// Bottom right.
		vertexes[2].ColorPicker = Color; 								// Bottom left.
		vertexes[3].ColorPicker = XMFLOAT4 ( 0.0f, 0.0f, 0.0f, 0.0f );	// Top right.
		break;
		// ALPHA_PICKER_HORIZONTAL
	case ALPHA_PICKER_VERTICAL:
		vertexes[0].TexCoord = XMFLOAT4 ( 0.0f, 0.0f, 0.0f, 1.0f ); //w = 1.0f - ColorPicker draw in shader as ColorPicker, not texture
		vertexes[1].TexCoord = XMFLOAT4 ( 1.0f, 1.0f, 0.0f, 1.0f );	//w = 1.0f - ColorPicker draw in shader as ColorPicker, not texture
		vertexes[2].TexCoord = XMFLOAT4 ( 0.0f, 1.0f, 0.0f, 1.0f );	//w = 1.0f - ColorPicker draw in shader as ColorPicker, not texture
		vertexes[3].TexCoord = XMFLOAT4 ( 0.0f, 0.0f, 0.0f, 1.0f );	//w = 1.0f - ColorPicker draw in shader as ColorPicker, not texture

		vertexes[0].ColorPicker = Color;								// Top left.
		vertexes[1].ColorPicker = XMFLOAT4 ( 0.0f, 0.0f, 0.0f, 0.0f );	// Bottom right.
		vertexes[2].ColorPicker = XMFLOAT4 ( 0.0f, 0.0f, 0.0f, 0.0f );	// Bottom left.
		vertexes[3].ColorPicker = Color;								// Top right.
	break;

	default:;
	}
}


void FlatObjectClass::ChangeMousePosition( int positionX, int positionY )
{
	int left, right, top, bottom;
	Vertex_FlatObject* vertexes;

	// If the position we are rendering this bitmap to has not changed then don't update the vertex buffer since it
	// currently has the correct parameters.
	if ( ( positionX == previousPosX ) && ( positionY == previousPosY ) )
	{
		return;
	}

	// If it has changed then update the position it is being rendered to.
	previousPosX = positionX;
	previousPosY = positionY;

	// Calculate the screen coordinates of the left side of the bitmap.
	left = (int)-_2DPixelXmax + positionX;

	// Calculate the screen coordinates of the right side of the bitmap.
	right = left + (int)ObjParam.z;//m_bitmapWidth;

	// Calculate the screen coordinates of the top of the bitmap.
	top = (int)_2DPixelYmax - positionY;

	// Calculate the screen coordinates of the bottom of the bitmap.
	bottom = top - (int)ObjParam.w; //m_bitmapHeight;

	// Create the vertex array.
	vertexes = new Vertex_FlatObject[4];

	vertexes[0].Position = XMFLOAT3( ( float ) left, ( float ) top, 0.0f );  // Top left.
	vertexes[0].TexCoord = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );

	vertexes[1].Position = XMFLOAT3( ( float ) right, ( float ) bottom, 0.0f );  // Bottom right.
	vertexes[1].TexCoord = XMFLOAT4( 1.0f, 1.0f, 0.0f, 0.0f );

	vertexes[2].Position = XMFLOAT3( ( float ) left, ( float ) bottom, 0.0f );  // Bottom left.
	vertexes[2].TexCoord = XMFLOAT4( 0.0f, 1.0f, 0.0f, 0.0f );

	vertexes[3].Position = XMFLOAT3( ( float ) right, ( float ) top, 0.0f );  // Top right.
	vertexes[3].TexCoord = XMFLOAT4( 1.0f, 0.0f, 0.0f, 0.0f );


	Buffers->FlatObjectVB->Update ( vertexes );

	delete[] vertexes;

}
//------------------------------------------------------------------------------------------------класс для рисования (Прямоугольных) обьектов 
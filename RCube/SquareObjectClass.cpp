#include "stdafx.h"
#include "SquareObjectClass.h"


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ класс для рисования (Прямоугольных) обьектов 

SquareObjectClass::SquareObjectClass()
{

//	memset(this, 0, sizeof(SquareObjectClass));
			Obj = nullptr;
	  D3DGC_Obj = nullptr;
	  ObjOriginalParam = ObjParam = { 0.0f, 0.0f, 0.0f, 0.0f };
	  previousPosX = 0;
	  previousPosY = 0;

}


HRESULT SquareObjectClass::Init(D3DGlobalContext* D3DGC,
	XMFLOAT4& _ObjParam, 
	ID3D11ShaderResourceView * TexCoord,
	int _TexCoordFlip
	){

	HRESULT Result = S_OK ;
	
	D3DGC_Obj = D3DGC;

	ObjOriginalParam = ObjParam = _ObjParam;

	TextureFlip = _TexCoordFlip;

	_2DPixelXmax = float(D3DGC_Obj->ScreenWidth / 2);
	_2DPixelYmax = float(D3DGC_Obj->ScreenHeight / 2);

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
		
	Vertex_FlatObject* vertices ;

	vertices = new Vertex_FlatObject[4];

	GenerateVertexes( vertices, TextureFlip );
	
	unsigned long indices[6] = { 0, 1, 2, 0, 3, 1 };

	Obj = new KF2DObjClass ;
	if ( Obj->Init( D3DGC, vertices, indices, TexCoord, 4, 6 ) != S_OK )
	{
		Result = E_FAIL;
	}

	delete [] vertices ;

	return Result;
}


void SquareObjectClass::Draw()
{

	Obj->Render();

}


SquareObjectClass::~SquareObjectClass(){

	RCUBE_DELETE( Obj );
}

void SquareObjectClass::SetObjectTexture (ID3D11ShaderResourceView* TexCoord){

	Obj->SetObjectTexture (TexCoord) ;
}


//  XMFLOAT4&   -     _Top , _Left , _Bottom , _Right
// Используется в классе анимации текстур
void SquareObjectClass::ResetObjectTexcord( XMFLOAT4& Position )
{
	Vertex_FlatObject* vertices ;
	// Create the vertex array.
	vertices = new Vertex_FlatObject[4];

	vertices[0].Position = XMFLOAT3( left, top, 0.0f );  // Top left.
	vertices[1].Position = XMFLOAT3( right, bottom, 0.0f );  // Bottom right.
	vertices[2].Position = XMFLOAT3( left, bottom, 0.0f );  // Bottom left.
	vertices[3].Position = XMFLOAT3( right, top, 0.0f );  // Top right.

	switch (TextureFlip)
	{
		// NO_FLIP
	case 0:
		vertices[0].TexCoord = XMFLOAT2( Position.y, Position.x );
		vertices[1].TexCoord = XMFLOAT2( Position.w, Position.z );
		vertices[2].TexCoord = XMFLOAT2( Position.y, Position.z );
		vertices[3].TexCoord = XMFLOAT2( Position.w, Position.x );
		break;
		// FLIP_HORIZONTAL
	case 1:
		vertices[0].TexCoord = XMFLOAT2( 1.0f, 0.0f );
		vertices[1].TexCoord = XMFLOAT2( 0.0f, 1.0f );
		vertices[2].TexCoord = XMFLOAT2( 1.0f, 1.0f );
		vertices[3].TexCoord = XMFLOAT2( 0.0f, 0.0f );
		break;
		// FLIP_VERTICAL
	case 2:
		vertices[0].TexCoord = XMFLOAT2( 0.0f, 0.0f );
		vertices[1].TexCoord = XMFLOAT2( 1.0f, 1.0f );
		vertices[2].TexCoord = XMFLOAT2( 0.0f, 1.0f );
		vertices[3].TexCoord = XMFLOAT2( 1.0f, 0.0f );
		break;
		// ROTATE90
	case 3:
		vertices[0].TexCoord = XMFLOAT2( 0.0f, 1.0f );
		vertices[1].TexCoord = XMFLOAT2( 1.0f, 0.0f );
		vertices[2].TexCoord = XMFLOAT2( 1.0f, 1.0f );
		vertices[3].TexCoord = XMFLOAT2( 0.0f, 0.0f );
		break;
		// ROTATE_90
	case 4:
		vertices[0].TexCoord = XMFLOAT2( 1.0f, 0.0f );
		vertices[1].TexCoord = XMFLOAT2( 0.0f, 1.0f );
		vertices[2].TexCoord = XMFLOAT2( 0.0f, 0.0f );
		vertices[3].TexCoord = XMFLOAT2( 1.0f, 1.0f );
		break;
		// FLIP_HORIZONTAL_ROTATE_90
	case 5:
		vertices[0].TexCoord = XMFLOAT2( 1.0f, 1.0f );
		vertices[1].TexCoord = XMFLOAT2( 0.0f, 0.0f );
		vertices[2].TexCoord = XMFLOAT2( 0.0f, 1.0f );
		vertices[3].TexCoord = XMFLOAT2( 1.0f, 0.0f );
		break;

	default:;
	}

	Obj->SetVertexBuffer( vertices ) ;

	delete [] vertices ;
}

ID3D11ShaderResourceView * SquareObjectClass::GetTexture(){

	return Obj->GetTexture () ;

}


void SquareObjectClass::SetObjParam(){

	UpdateVertexPos();

	Vertex_FlatObject* vertices ;

	vertices = new Vertex_FlatObject[4];

	GenerateVertexes( vertices, TextureFlip );

	Obj->SetVertexBuffer( vertices ) ;
	delete [] vertices ;

}


void SquareObjectClass::UpdateVertexPos()
{
	left = -_2DPixelXmax + ObjParam.x;
	right = left + ObjParam.z;
	top = _2DPixelYmax - ObjParam.y;
	bottom = top - ObjParam.w;
}


void SquareObjectClass::ResizeObject(float Percent)
{
	ObjParam.z = ObjOriginalParam.z * (Percent / 100);
	ObjParam.w = ObjOriginalParam.w * (Percent / 100);
	SetObjParam();
}


void SquareObjectClass::SetOriginalSize(void)
{
	ObjParam.z = ObjOriginalParam.z;
	ObjParam.w = ObjOriginalParam.w;
}


// Генерация вертексов и текстуры объекта в соответствии с поворотом текстуры 
// Для кнопок ScrollBar, чтобы можно было использовать одну текстуру для Min и Max кнопки
void SquareObjectClass::GenerateVertexes( Vertex_FlatObject* vertices, int TexCoordFlip )
{
	vertices[0].Position = XMFLOAT3( left, top, 0.0f );  // Top left.
	vertices[1].Position = XMFLOAT3( right, bottom, 0.0f );  // Bottom right.
	vertices[2].Position = XMFLOAT3( left, bottom, 0.0f );  // Bottom left.
	vertices[3].Position = XMFLOAT3( right, top, 0.0f );  // Top right.

	switch (TexCoordFlip)
	{
		// NO_FLIP
	case 0:
		vertices[0].TexCoord = XMFLOAT2( 0.0f, 0.0f );
		vertices[1].TexCoord = XMFLOAT2( 1.0f, 1.0f );
		vertices[2].TexCoord = XMFLOAT2( 0.0f, 1.0f );
		vertices[3].TexCoord = XMFLOAT2( 1.0f, 0.0f );
		break;
		// FLIP_HORIZONTAL
	case 1:
		vertices[0].TexCoord = XMFLOAT2( 1.0f, 0.0f );
		vertices[1].TexCoord = XMFLOAT2( 0.0f, 1.0f );
		vertices[2].TexCoord = XMFLOAT2( 1.0f, 1.0f );
		vertices[3].TexCoord = XMFLOAT2( 0.0f, 0.0f );
		break;
		// FLIP_VERTICAL
	case 2:
		vertices[0].TexCoord = XMFLOAT2( 0.0f, 0.0f );
		vertices[1].TexCoord = XMFLOAT2( 1.0f, 1.0f );
		vertices[2].TexCoord = XMFLOAT2( 0.0f, 1.0f );
		vertices[3].TexCoord = XMFLOAT2( 1.0f, 0.0f );
		break;
		// ROTATE90
	case 3:
		vertices[0].TexCoord = XMFLOAT2( 0.0f, 1.0f );
		vertices[1].TexCoord = XMFLOAT2( 1.0f, 0.0f );
		vertices[2].TexCoord = XMFLOAT2( 1.0f, 1.0f );
		vertices[3].TexCoord = XMFLOAT2( 0.0f, 0.0f );
		break;
		// ROTATE_90
	case 4:
		vertices[0].TexCoord = XMFLOAT2( 1.0f, 0.0f );
		vertices[1].TexCoord = XMFLOAT2( 0.0f, 1.0f );
		vertices[2].TexCoord = XMFLOAT2( 0.0f, 0.0f );
		vertices[3].TexCoord = XMFLOAT2( 1.0f, 1.0f );
		break;
		// FLIP_HORIZONTAL_ROTATE_90
	case 5:
		vertices[0].TexCoord = XMFLOAT2( 1.0f, 1.0f );
		vertices[1].TexCoord = XMFLOAT2( 0.0f, 0.0f );
		vertices[2].TexCoord = XMFLOAT2( 0.0f, 1.0f );
		vertices[3].TexCoord = XMFLOAT2( 1.0f, 0.0f );
		break;
	default:;
	}
}


void SquareObjectClass::ChangeMousePosition( int positionX, int positionY )
{
	int left, right, top, bottom;
	Vertex_FlatObject* vertices;

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
	vertices = new Vertex_FlatObject[4];

	vertices[0].Position = XMFLOAT3( ( float ) left, ( float ) top, 0.0f );  // Top left.
	vertices[0].TexCoord = XMFLOAT2( 0.0f, 0.0f );

	vertices[1].Position = XMFLOAT3( ( float ) right, ( float ) bottom, 0.0f );  // Bottom right.
	vertices[1].TexCoord = XMFLOAT2( 1.0f, 1.0f );

	vertices[2].Position = XMFLOAT3( ( float ) left, ( float ) bottom, 0.0f );  // Bottom left.
	vertices[2].TexCoord = XMFLOAT2( 0.0f, 1.0f );

	vertices[3].Position = XMFLOAT3( ( float ) right, ( float ) top, 0.0f );  // Top right.
	vertices[3].TexCoord = XMFLOAT2( 1.0f, 0.0f );


	Obj->SetVertexBuffer( vertices );

	delete[] vertices;

}
//------------------------------------------------------------------------------------------------класс для рисования (Прямоугольных) обьектов 
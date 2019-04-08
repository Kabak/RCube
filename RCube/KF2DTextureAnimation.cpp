#include "stdafx.h"
#include "KF2DTextureAnimation.h"
#include "ResourceManager.h"

KF2DTextureAnimation::KF2DTextureAnimation()
{

	  TextcordTop = nullptr;
	 TextcordLeft = nullptr;
	AnimeTexture  = nullptr;
		  Enabled = true;
	  CurentFrame = 0; 
		  PlayFPS = 24.0f;
	   TimePassed = 0.0f;

	   UX_Amount  = 0;			// Ширина текстуры Width X U
	   VY_Amount  = 0;			// Высота текстуры Height Y V
}

KF2DTextureAnimation::~KF2DTextureAnimation()
{
	ResManager->Delete_Flat_ObjectBuffers ( AnimeTextureBufferIndex );
	RCUBE_DELETE ( AnimeTexture )
	RCUBE_ARR_DELETE ( TextcordTop )
	RCUBE_ARR_DELETE ( TextcordLeft )
}

HRESULT KF2DTextureAnimation::Init(HWND hwnd, 
	D3DGlobalContext* D3DGC, 
	int _UX_Amount,							// Количество строк в текстуре анимации
	int _VY_Amount,							// Количестко столбцов в текстуре анимации
	ID3D11ShaderResourceView * animTexture,	// Текстура с анимацией
	int DrawShaderIndex,					// Номер шейдера которым нужно рисовать
//	int ReturnShaderIndex,					// Номер шейдера на который нужно вернуть после анимации
//	Flat_Obj_Buffers* _Buffers,				// Менеджер шейдеров для переключения шейдера
	XMFLOAT4& _ObjParam,					// x,y - позиция X,Y , z - Width , w - Height элемента
	ResourceManager * _GlobalResourceManager
	)
{

	HRESULT result;
	
	ResManager = _GlobalResourceManager;

	ShaderForDraw = DrawShaderIndex;
//	ShaderReturnTo = ReturnShaderIndex;

	ObjParam = _ObjParam;

	UX_Amount = _UX_Amount;
	VY_Amount = _VY_Amount;

	OneFrameWidth = 1.0f / UX_Amount;
	OneFrameHeight = 1.0f / VY_Amount;

	NumOfAnimeFrams = UX_Amount * VY_Amount;


	TextcordTop = new float[ VY_Amount ];
	TextcordLeft = new float[ UX_Amount ];


	int UX = 0;
	int VY = 0;
	while ( VY < VY_Amount )
	{
		TextcordTop[VY] = static_cast<float>( VY * OneFrameHeight );
		++VY;
	}

	while ( UX <  UX_Amount )
	{
		TextcordLeft[UX] = static_cast<float>( UX * OneFrameWidth );
		++UX;
	}

	AnimeTextureBufferIndex = ResManager->Create_Flat_Obj_Buffers ( NO_CPU_ACCESS_BUFFER, 4, 6, nullptr );

	AnimeTexture = new FlatObjectClass;
	result = AnimeTexture->Init( D3DGC->ScreenWidth, D3DGC->ScreenHeight, ObjParam, animTexture, ANIMATION_TEXTURE, ResManager->Get_Flat_ObjectBuffers_ByIndex ( AnimeTextureBufferIndex ) );
	if (result == E_FAIL){

		MessageBox(hwnd, L"Can't create KFAnimation.", L"Error", MB_OK);
		return E_FAIL;

	}

	return S_OK;
}


void KF2DTextureAnimation::SetFPSpeed( float FPS )
{
	PlayFPS = FPS;
}

void KF2DTextureAnimation::Frame( FPSTimers &fpstimers )
{
	if ( fpstimers.FpsRate > 0 )
		TimePassed += 1.0f / float( fpstimers.FpsRate );

	if ( TimePassed > 1.0f / PlayFPS ) //  Скорость смены кадров 24 кадра в секунду по умолчанию 
	{
		TimePassed = 0.0f;
		
		if ( CurentFrame == NumOfAnimeFrams )
			CurentFrame = 0;
		else
			++CurentFrame;
	}


	int Column;
	int Line;

	Line = CurentFrame / UX_Amount;

	Line > 0 ? Column = CurentFrame - Line * UX_Amount : Column = CurentFrame;

	float Top = TextcordTop[Line];
	float Left = TextcordLeft[Column];

	XMFLOAT4 Data =
	{
		Top,
		Left,
		Top + OneFrameHeight,
		Left + OneFrameWidth
	};

		AnimeTexture->ResetObjectTexcord( Data );
}


void KF2DTextureAnimation::SetAnimeParam( XMFLOAT4& Param )
{
	AnimeTexture->ObjParam = Param;
	AnimeTexture->SetObjParam();
}


void KF2DTextureAnimation::SetAnimePos(XMFLOAT2& Pos)
{
	AnimeTexture->ObjParam.x = Pos.x;
	AnimeTexture->ObjParam.y = Pos.y;
	AnimeTexture->SetObjParam();
}


void KF2DTextureAnimation::SetAnimeSize(XMFLOAT2& Size)
{
	AnimeTexture->ObjParam.z = Size.x;
	AnimeTexture->ObjParam.w = Size.y;
	AnimeTexture->SetObjParam();
}
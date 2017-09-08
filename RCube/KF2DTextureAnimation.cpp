#include "stdafx.h"
#include "KF2DTextureAnimation.h"

KF2DTextureAnimation::KF2DTextureAnimation()
{

	  TextcordTop = nullptr;
	 TextcordLeft = nullptr;
	AnimeTextures = nullptr;
		  Enabled = true;
	  CurentFrame = 0; 
		  PlayFPS = 24.0f;
	   TimePassed = 0.0f;

	   UX_Amount  = 0;			// Ширина текстуры Width X U
	   VY_Amount  = 0;			// Высота текстуры Height Y V
}

KF2DTextureAnimation::~KF2DTextureAnimation()
{
	RCUBE_DELETE ( AnimeTextures )
	RCUBE_ARR_DELETE ( TextcordTop )
	RCUBE_ARR_DELETE ( TextcordLeft )
}

HRESULT KF2DTextureAnimation::Init(HWND hwnd, 
	D3DGlobalContext* D3DGC, 
	int _UX_Amount,							// Количество строк в текстуре анимации
	int _VY_Amount,							// Количестко столбцов в текстуре анимации
	ID3D11ShaderResourceView * animTexture,	// Текстура с анимацией
	int DrawShaderIndex,					// Номер шейдера которым нужно рисовать
	int ReturnShaderIndex,					// Номер шейдера на который нужно вернуть после анимации
	KFResourceManager *_ShaderManager,		// Менеджер шейдеров для переключения шейдера
	XMFLOAT4& _ObjParam,					// x,y - позиция X,Y , z - Width , w - Height элемента
	ID3D10Blob * WorkBlob					// Blob от шейдера для совмещения с Layout
	)
{

	HRESULT result;
	
	ShaderForDraw = DrawShaderIndex;
	ShaderReturnTo = ReturnShaderIndex;
	ShaderManager = _ShaderManager;

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

//	float h = TextcordTop[0], i = TextcordLeft[0];

	AnimeTextures = new SquareObjectClass;
	result = AnimeTextures->Init(D3DGC, ObjParam, animTexture, 0, WorkBlob );
	if (result == E_FAIL){

		MessageBox(hwnd, L"Не могу создать место для отрисовки. KFAnimation.cpp.", L"Error", MB_OK);
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

		AnimeTextures->ResetObjectTexcord( Data );
}


void KF2DTextureAnimation::Draw()
{
	if ( Enabled )
	{
		if ( ShaderForDraw > -1 )
			ShaderManager->SetActiveShadersInProgramm( ShaderForDraw );
		
		AnimeTextures->Draw();

		if ( ShaderReturnTo > -1 )
			ShaderManager->SetActiveShadersInProgramm( ShaderReturnTo );
	}

}

void KF2DTextureAnimation::SetAnimeParam( XMFLOAT4& Param )
{
	AnimeTextures->ObjParam = Param;
	AnimeTextures->SetObjParam();
}


void KF2DTextureAnimation::SetAnimePos(XMFLOAT2& Pos)
{
	AnimeTextures->ObjParam.x = Pos.x;
	AnimeTextures->ObjParam.y = Pos.y;
	AnimeTextures->SetObjParam();
}


void KF2DTextureAnimation::SetAnimeSize(XMFLOAT2& Size)
{
	AnimeTextures->ObjParam.z = Size.x;
	AnimeTextures->ObjParam.w = Size.y;
	AnimeTextures->SetObjParam();
}
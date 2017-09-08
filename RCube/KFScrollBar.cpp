#include "stdafx.h"
#include "KFScrollBar.h"

HRESULT ScrollBarClass::Init(HWND hwnd, int m_screenWidth, int m_screenHeight, int WindowPosX, int WindowPosY, float positionX
	, float positionY, ID3D11ShaderResourceView * OsnTexture, ID3D11ShaderResourceView * TravellerTexture
	, ID3D11ShaderResourceView * ButtonsTexture, bool * ReSetsCont, ID3D11Resource * res, XMMATRIX & OrthoMatrix
	, D3DGlobalContext* D3DGC, XMMATRIX & BaseViewMatrix, ID3D10Blob * WorkBlob ,  int HorOrVer , int Indention , bool ButtonsShow , 
	int TravellerLength){


	HRESULT result = S_OK;
	//	ID3D11Resource * Res ;

	// общие переменные для работы с меню
	ReSetsControl = ReSetsCont;
	startScreenWidth = m_screenWidth;
	startScreenHeigth = m_screenHeight;
	Enalbled = true;
	g_WindowPosX = WindowPosX;
	g_WindowPosY = WindowPosY;
	x = positionX;
	y = positionY;
	CordInPixX = (int)x * m_screenWidth;
	CordInPixY = (int)y * m_screenHeight;

	g_ScreenWidth = m_screenWidth;
	g_ScreenHeight = m_screenHeight;

	g_Device = D3DGC->m_device;
	g_DeviceContext = D3DGC->m_deviceContext;
	// общие переменные для работы с меню

	// специальные переменные скролл бара
	g_HorOrVer = HorOrVer;
	g_Indention = Indention;
	IsButtonShow = ButtonsShow;
	// специальные переменные скролл бара

	// текстуры
	OsnButtonsTexture = ButtonsTexture;
	OsnTravellerTexture = TravellerTexture;
	OsnBarTexture = OsnTexture;
	// текстуры

	// определение размеров текстуры
	auto txt = reinterpret_cast<ID3D11Texture2D*>(res);
	D3D11_TEXTURE2D_DESC desc;
	txt->GetDesc(&desc);
	              _Width = (float)desc.Width;
	              _Heigth = (float)desc.Height;
	// определение размеров текстуры

	Previos_Width = (float)_Width;
	Previos_Heigth = (float)_Heigth;


	left = (float)((m_screenWidth / 2) * -1) + (float)CordInPixX;

	// Calculate the screen coordinates of the right side of the bitmap.
	right = left + (float)_Width;

	// Calculate the screen coordinates of the top of the bitmap.
	top = (float)(m_screenHeight / 2) - (float)CordInPixY;

	// Calculate the screen coordinates of the bottom of the bitmap.
	bottom = top - (float)_Heigth;


	Previostop = (int)top;
	Previosright = (int)right;
	Previosbottom = (int)bottom;
	Previosleft = (int)left;


	InterfaceVertexType* vertices , *Button1Vertexes , *Button2Vertexes;
	// Create the vertex array.
	vertices = new InterfaceVertexType[6];
	if (!vertices)
	{
		return E_FAIL;
	}


    TravellerVertexes = new InterfaceVertexType[6];
	if (!TravellerVertexes) {
		return E_FAIL;
	}

	Button1Vertexes = new InterfaceVertexType[6];
	if (!Button1Vertexes)
	{
		return E_FAIL;
	}

	Button2Vertexes = new InterfaceVertexType[6];
	if (!Button1Vertexes)
	{
		return E_FAIL;
	}

	//если горизонтально
	if (HorOrVer >= 0) {

		// Load the vertex array with data.
		// First triangle.
		// создание самой строки
		vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
		vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

		vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
		vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

		vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
		vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

		// Second triangle.
		vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
		vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

		vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
		vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

		vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
		vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

		// создание  левой кнопки
		Button1Vertexes[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
		Button1Vertexes[0].texture = XMFLOAT2(1.0f, 0.0f);

		Button1Vertexes[1].position = XMFLOAT3(left + _Heigth, bottom, 0.0f);  // Bottom right.
		Button1Vertexes[1].texture = XMFLOAT2(0.0f, 1.0f);

		Button1Vertexes[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
		Button1Vertexes[2].texture = XMFLOAT2(0.0f, 0.0f);

		// Second triangle.
		Button1Vertexes[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
		Button1Vertexes[3].texture = XMFLOAT2(1.0f, 0.0f);

		Button1Vertexes[4].position = XMFLOAT3(left + _Heigth, top, 0.0f);  // Top right.
		Button1Vertexes[4].texture = XMFLOAT2(1.0f, 1.0f);

		Button1Vertexes[5].position = XMFLOAT3(left + _Heigth, bottom, 0.0f);  // Bottom right.
		Button1Vertexes[5].texture = XMFLOAT2(0.0f, 1.0f);

		// создание  правой кнопки
		Button2Vertexes[0].position = XMFLOAT3(right - _Heigth, top, 0.0f);  // Top left.
		Button2Vertexes[0].texture = XMFLOAT2(0.0f, 1.0f);

		Button2Vertexes[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
		Button2Vertexes[1].texture = XMFLOAT2(1.0f, 0.0f);

		Button2Vertexes[2].position = XMFLOAT3(right - _Heigth, bottom, 0.0f);  // Bottom left.
		Button2Vertexes[2].texture = XMFLOAT2(1.0f, 1.0f);

		// Second triangle.
		Button2Vertexes[3].position = XMFLOAT3(right - _Heigth, top, 0.0f);  // Top left.
		Button2Vertexes[3].texture = XMFLOAT2(0.0f, 1.0f);

		Button2Vertexes[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
		Button2Vertexes[4].texture = XMFLOAT2(0.0f, 0.0f);

		Button2Vertexes[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
		Button2Vertexes[5].texture = XMFLOAT2(1.0f, 0.0f);

		// создание бегунка

		// надо рассчитать размер бегунка

		// это я рассчитываю свобобное пространство в строке для бегунка
		int ChainlessBarArea = (int)(_Width - (_Heigth * 2));
		// а это рассчет самой длинны бегунка
		// определение размеров текстуры
		TravellerLenght = TravellerLength;


		TravellerVertexes[0].position = XMFLOAT3(left + _Heigth, top, 0.0f);  // Top left.
		TravellerVertexes[0].texture = XMFLOAT2(0.0f, 0.0f);

		TravellerVertexes[1].position = XMFLOAT3(left + _Heigth + DrawingTravellerLenght, bottom, 0.0f);  // Bottom right.
		TravellerVertexes[1].texture = XMFLOAT2(1.0f, 1.0f);

		TravellerVertexes[2].position = XMFLOAT3(left + _Heigth, bottom, 0.0f);  // Bottom left.
		TravellerVertexes[2].texture = XMFLOAT2(0.0f, 1.0f);

		// Second triangle.
		TravellerVertexes[3].position = XMFLOAT3(left + _Heigth, top, 0.0f);  // Top left.
		TravellerVertexes[3].texture = XMFLOAT2(0.0f, 0.0f);

		TravellerVertexes[4].position = XMFLOAT3(left + _Heigth + DrawingTravellerLenght, top, 0.0f);  // Top right.
		TravellerVertexes[4].texture = XMFLOAT2(1.0f, 0.0f);

		TravellerVertexes[5].position = XMFLOAT3(left + _Heigth + DrawingTravellerLenght, bottom, 0.0f);  // Bottom right.
		TravellerVertexes[5].texture = XMFLOAT2(1.0f, 1.0f);

	}


	//если вертикально
	if (HorOrVer < 0) {

		// Load the vertex array with data.
		// First triangle.
		// создание самой строки
		vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
		vertices[0].texture = XMFLOAT2(0.0f, 1.0f);

		vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
		vertices[1].texture = XMFLOAT2(1.0f, 0.0f);

		vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
		vertices[2].texture = XMFLOAT2(1.0f, 1.0f);

		// Second triangle.
		vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
		vertices[3].texture = XMFLOAT2(0.0f, 1.0f);

		vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
		vertices[4].texture = XMFLOAT2(0.0f, 0.0f);

		vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
		vertices[5].texture = XMFLOAT2(1.0f, 0.0f);

		// создание верхней кнопки 
		Button1Vertexes[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
		Button1Vertexes[0].texture = XMFLOAT2(0.0f, 0.0f);

		Button1Vertexes[1].position = XMFLOAT3(right, top - _Width, 0.0f);  // Bottom right.
		Button1Vertexes[1].texture = XMFLOAT2(1.0f, 1.0f);

		Button1Vertexes[2].position = XMFLOAT3(left, top - _Width, 0.0f);  // Bottom left.
		Button1Vertexes[2].texture = XMFLOAT2(0.0f, 1.0f);

		// Second triangle.
		Button1Vertexes[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
		Button1Vertexes[3].texture = XMFLOAT2(0.0f, 0.0f);

		Button1Vertexes[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
		Button1Vertexes[4].texture = XMFLOAT2(1.0f, 0.0f);

		Button1Vertexes[5].position = XMFLOAT3(right, top - _Width, 0.0f);  // Bottom right.
		Button1Vertexes[5].texture = XMFLOAT2(1.0f, 1.0f);

		// создание нижней кнопки 
		Button2Vertexes[0].position = XMFLOAT3(left, bottom + _Width, 0.0f);  // Top left.
		Button2Vertexes[0].texture = XMFLOAT2(1.0f, 1.0f);

		Button2Vertexes[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
		Button2Vertexes[1].texture = XMFLOAT2(0.0f, 0.0f);

		Button2Vertexes[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
		Button2Vertexes[2].texture = XMFLOAT2(1.0f, 0.0f);

		// Second triangle.
		Button2Vertexes[3].position = XMFLOAT3(left, bottom + _Width, 0.0f);  // Top left.
		Button2Vertexes[3].texture = XMFLOAT2(1.0f, 1.0f);

		Button2Vertexes[4].position = XMFLOAT3(right, bottom + _Width, 0.0f);  // Top right.
		Button2Vertexes[4].texture = XMFLOAT2(0.0f, 1.0f);

		Button2Vertexes[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
		Button2Vertexes[5].texture = XMFLOAT2(0.0f, 0.0f);

		// это я рассчитываю свобобное пространство в строке для бегунка
		int ChainlessBarArea = (int)(_Heigth - (_Width * 2));
		// а это рассчет самой длинны бегунка для программы
		// определение размеров текстуры

		TravellerLenght = TravellerLength;

		TravellerVertexes[0].position = XMFLOAT3(left, top - _Width, 0.0f);  // Top left.
		TravellerVertexes[0].texture = XMFLOAT2(0.0f, 0.0f);

		TravellerVertexes[1].position = XMFLOAT3(right, top - _Width - DrawingTravellerLenght, 0.0f);  // Bottom right.
		TravellerVertexes[1].texture = XMFLOAT2(1.0f, 1.0f);

		TravellerVertexes[2].position = XMFLOAT3(left, top - _Width - DrawingTravellerLenght, 0.0f);  // Bottom left.
		TravellerVertexes[2].texture = XMFLOAT2(0.0f, 1.0f);

		// Second triangle.
		TravellerVertexes[3].position = XMFLOAT3(left, top - _Width, 0.0f);  // Top left.
		TravellerVertexes[3].texture = XMFLOAT2(0.0f, 0.0f);

		TravellerVertexes[4].position = XMFLOAT3(right, top - _Width, 0.0f);  // Top right.
		TravellerVertexes[4].texture = XMFLOAT2(1.0f, 0.0f);

		TravellerVertexes[5].position = XMFLOAT3(right, top - _Width - DrawingTravellerLenght, 0.0f);  // Bottom right.
		TravellerVertexes[5].texture = XMFLOAT2(1.0f, 1.0f);

	}

	unsigned long indices[6];
	int i = 0;

	for (i = 0; i<6; ++i)
	{
		indices[i] = i;
	}

	Obj = new KF2DObjClass;
	Button1 = new KF2DObjClass;
	Button2 = new KF2DObjClass;
	Traveller = new KF2DObjClass;
	Obj->Init(hwnd, vertices, indices, OsnTexture, g_Device, 6, 6, g_DeviceContext, WorkBlob);
	Button1->Init(hwnd, Button1Vertexes, indices, ButtonsTexture, g_Device, 6, 6, g_DeviceContext, WorkBlob);
	Button2->Init(hwnd, Button2Vertexes, indices, ButtonsTexture, g_Device, 6, 6, g_DeviceContext, WorkBlob);
	Traveller->Init(hwnd, TravellerVertexes, indices, TravellerTexture, g_Device, 6, 6, g_DeviceContext, WorkBlob);

	delete[] vertices;
	delete[] Button1Vertexes;
	delete[] Button2Vertexes;

	MidleTraveller = DrawingTravellerLenght / 2;

	// если задан пункт отображать кнопки
	if (IsButtonShow) {
		// эти нужны для глобальных кординат
		ChainlesBarTop = Top + (int)_Width + Indention;
		ChainlessBarBottom = Bottom - (int)_Width - Indention;
		ChainlessBarLeft = Left + (int)_Heigth + Indention;
		ChainlessBarRight = Right - (int)_Heigth - Indention;
		g_TravellerLeftInGlobalCord = ChainlessBarLeft;
		g_TravellerTopInGlobalCord = ChainlesBarTop;

		// а эти для кординат в системе DirectX
		ChainlesBarTop2 = (int)top - (int)_Width - Indention;
		TravellerTop = ChainlesBarTop2;
		ChainlessBarBottom2 = (int)bottom + (int)_Width + Indention;
		ChainlessBarLeft2 = (int)left + (int)_Heigth + Indention;
		TravellerLeft = ChainlessBarLeft2;
		ChainlessBarRight2 = (int)right - (int)_Heigth - Indention;
	}
	// если задан пункт неотображать кнопки
	else {
		ChainlesBarTop = Top + Indention;
		ChainlessBarBottom = Bottom - Indention;
		ChainlessBarLeft = Left + Indention;
		ChainlessBarRight = Right - Indention;

		// а эти для кординат в системе DirectX
		ChainlesBarTop2 = (int)top - Indention;
		TravellerTop = ChainlesBarTop2;
		ChainlessBarBottom2 = (int)bottom + Indention;
		ChainlessBarLeft2 = (int)left + Indention;
		TravellerLeft = ChainlessBarLeft2;
		ChainlessBarRight2 = (int)right - Indention;
	}

	IsMouseDownOnObjec = true;

	return result;
}

HRESULT ScrollBarClass::Draw() {

	HRESULT Result = S_OK;

	if (PreviosX != PreviosX || PreviosY == PreviosY || _Width == Previos_Width || _Heigth == Previos_Heigth )
	SetPos();

	Obj->Render();
	if (IsButtonShow) {
		Button1->Render();
		Button2->Render();
	}
	Traveller->Render();

	return S_OK;

}

ScrollBarClass::~ScrollBarClass() {
	if (Button1 != nullptr) {
		delete Button1;
		Button1 = 0;
	}
	if (Button2 != nullptr) {
		delete Button2;
	}
	if (Traveller != nullptr) {
		delete Traveller;
		Traveller = 0;
	}

	delete[] TravellerVertexes;
}

void ScrollBarClass::SetIsMouseOnButtonTexture(ID3D11ShaderResourceView * Texture) {
	IsMouseOnButtonTexture = Texture;

}

void ScrollBarClass::SetIsButtonsPressTexture(ID3D11ShaderResourceView * Texture) {

	IsButtonPressTexture = Texture;

}

void ScrollBarClass::SetIsBarNotEnalbledButtonsTexture(ID3D11ShaderResourceView * Texture) {

	IsBarNotEnalbledButtonsTexture = Texture;

}

void ScrollBarClass::SetTravellerMouseOnTexture(ID3D11ShaderResourceView * Texture) {

	IsMouseOnTravellerTexture = Texture;

}

void ScrollBarClass::SetTravellerPressTexture(ID3D11ShaderResourceView * Texture) {

	IsTravellerPressTexture = Texture;

}

void ScrollBarClass::SetTravellerNotEnalbledTexture(ID3D11ShaderResourceView * Texture) {

	IsNotEnalbledTravellerTexture = Texture;

}

void ScrollBarClass::SetBarNotEnalbledTexture(ID3D11ShaderResourceView * Texture) {

	NotEnalbledBarTexture = Texture;

}

int ScrollBarClass::GetBarState() {
	int result;

	if (g_HorOrVer >= 0) {
		int TravellerLeftInLocalCord = g_TravellerLeftInGlobalCord - ChainlessBarLeft,
			BarLenght = ChainlessBarRight - ChainlessBarLeft;
		result = TravellerLeftInLocalCord * 100;
		result = result / BarLenght;
	}
	if (g_HorOrVer < 0) {
		int TravellerTopInLocalCord = g_TravellerTopInGlobalCord - ChainlesBarTop,
			BarLenght = ChainlessBarLeft + ChainlessBarBottom;
		result = TravellerTopInLocalCord * 100;
		result = result / BarLenght;
	}

	return result;
}

HRESULT ScrollBarClass::SetPos() {

	CordInPixX = int(x  * startScreenWidth);
	CordInPixY = int(y * startScreenHeigth);

	PreviosX = x;
	PreviosY = y;
	Previos_Heigth = _Heigth;
	Previos_Width = _Width;


	InterfaceVertexType* vertices, *Button1Vertexes, *Button2Vertexes;
	// Create the vertex array.
	vertices = new InterfaceVertexType[6];
	if (!vertices)
	{
		return E_FAIL;
	}

	Button1Vertexes = new InterfaceVertexType[6];
	if (!Button1Vertexes)
	{
		return E_FAIL;
	}

	Button2Vertexes = new InterfaceVertexType[6];
	if (!Button1Vertexes)
	{
		return E_FAIL;
	}

	TravellerVertexes = new InterfaceVertexType[6];
	if (!TravellerVertexes) {
		return E_FAIL;
	}


	// создание вертексного буфера для обьекта 
	// Calculate the screen coordinates of the left side of the bitmap.
	left = (float)((startScreenWidth / 2) * -1) + (float)CordInPixX;

	// Calculate the screen coordinates of the right side of the bitmap.
	right = left + _Width;

	// Calculate the screen coordinates of the top of the bitmap.
	top = (float)(startScreenHeigth / 2) - (float)CordInPixY;

	// Calculate the screen coordinates of the bottom of the bitmap.
	bottom = top - _Heigth;

	Top += (int)top - Previostop;
	Bottom += (int)bottom - Previosbottom;
	Right += (int)right - Previosright;
	Left += (int)left - Previosleft;

	Previostop = (int)top;
	Previosright = (int)right;
	Previosbottom = (int)bottom;
	Previosleft = (int)left;

	//если горизонтально
	if (g_HorOrVer >= 0) {

		// Load the vertex array with data.
		// First triangle.
		// создание самой строки
		vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
		vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

		vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
		vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

		vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
		vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

		// Second triangle.
		vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
		vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

		vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
		vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

		vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
		vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

		// создание  левой кнопки
		Button1Vertexes[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
		Button1Vertexes[0].texture = XMFLOAT2(1.0f, 0.0f);

		Button1Vertexes[1].position = XMFLOAT3(left + _Heigth, bottom, 0.0f);  // Bottom right.
		Button1Vertexes[1].texture = XMFLOAT2(0.0f, 1.0f);

		Button1Vertexes[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
		Button1Vertexes[2].texture = XMFLOAT2(0.0f, 0.0f);

		// Second triangle.
		Button1Vertexes[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
		Button1Vertexes[3].texture = XMFLOAT2(1.0f, 0.0f);

		Button1Vertexes[4].position = XMFLOAT3(left + _Heigth, top, 0.0f);  // Top right.
		Button1Vertexes[4].texture = XMFLOAT2(1.0f, 1.0f);

		Button1Vertexes[5].position = XMFLOAT3(left + _Heigth, bottom, 0.0f);  // Bottom right.
		Button1Vertexes[5].texture = XMFLOAT2(0.0f, 1.0f);

		// создание  правой кнопки
		Button2Vertexes[0].position = XMFLOAT3(right - _Heigth, top, 0.0f);  // Top left.
		Button2Vertexes[0].texture = XMFLOAT2(0.0f, 1.0f);

		Button2Vertexes[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
		Button2Vertexes[1].texture = XMFLOAT2(1.0f, 0.0f);

		Button2Vertexes[2].position = XMFLOAT3(right - _Heigth, bottom, 0.0f);  // Bottom left.
		Button2Vertexes[2].texture = XMFLOAT2(1.0f, 1.0f);

		// Second triangle.
		Button2Vertexes[3].position = XMFLOAT3(right - _Heigth, top, 0.0f);  // Top left.
		Button2Vertexes[3].texture = XMFLOAT2(0.0f, 1.0f);

		Button2Vertexes[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
		Button2Vertexes[4].texture = XMFLOAT2(0.0f, 0.0f);

		Button2Vertexes[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
		Button2Vertexes[5].texture = XMFLOAT2(1.0f, 0.0f);

		// создание бегунка

		// надо рассчитать размер бегунка

		// это я рассчитываю свобобное пространство в строке для бегунка
		int ChainlessBarArea = (int)(_Width - (_Heigth * 2));
		// а это рассчет самой длинны бегунка
	//	TravellerLenght = ChainlessBarArea / g_NumAlveola;
		if (TravellerLenght > 1)
			DrawingTravellerLenght = TravellerLenght;
		else {
			DrawingTravellerLenght = 1;
			TravellerLenght = 1;
		}



		TravellerVertexes[0].position = XMFLOAT3(left + _Heigth, top, 0.0f);  // Top left.
		TravellerVertexes[0].texture = XMFLOAT2(0.0f, 0.0f);

		TravellerVertexes[1].position = XMFLOAT3(left + _Heigth + DrawingTravellerLenght, bottom, 0.0f);  // Bottom right.
		TravellerVertexes[1].texture = XMFLOAT2(1.0f, 1.0f);

		TravellerVertexes[2].position = XMFLOAT3(left + _Heigth, bottom, 0.0f);  // Bottom left.
		TravellerVertexes[2].texture = XMFLOAT2(0.0f, 1.0f);

		// Second triangle.
		TravellerVertexes[3].position = XMFLOAT3(left + _Heigth, top, 0.0f);  // Top left.
		TravellerVertexes[3].texture = XMFLOAT2(0.0f, 0.0f);

		TravellerVertexes[4].position = XMFLOAT3(left + _Heigth + DrawingTravellerLenght, top, 0.0f);  // Top right.
		TravellerVertexes[4].texture = XMFLOAT2(1.0f, 0.0f);

		TravellerVertexes[5].position = XMFLOAT3(left + _Heigth + DrawingTravellerLenght, bottom, 0.0f);  // Bottom right.
		TravellerVertexes[5].texture = XMFLOAT2(1.0f, 1.0f);

	}


	//если вертикально
	if (g_HorOrVer < 0) {

		// Load the vertex array with data.
		// First triangle.
		// создание самой строки
		vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
		vertices[0].texture = XMFLOAT2(0.0f, 1.0f);

		vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
		vertices[1].texture = XMFLOAT2(1.0f, 0.0f);

		vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
		vertices[2].texture = XMFLOAT2(1.0f, 1.0f);

		// Second triangle.
		vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
		vertices[3].texture = XMFLOAT2(0.0f, 1.0f);

		vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
		vertices[4].texture = XMFLOAT2(0.0f, 0.0f);

		vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
		vertices[5].texture = XMFLOAT2(1.0f, 0.0f);

		// создание верхней кнопки 
		Button1Vertexes[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
		Button1Vertexes[0].texture = XMFLOAT2(0.0f, 0.0f);

		Button1Vertexes[1].position = XMFLOAT3(right, top - _Width, 0.0f);  // Bottom right.
		Button1Vertexes[1].texture = XMFLOAT2(1.0f, 1.0f);

		Button1Vertexes[2].position = XMFLOAT3(left, top - _Width, 0.0f);  // Bottom left.
		Button1Vertexes[2].texture = XMFLOAT2(0.0f, 1.0f);

		// Second triangle.
		Button1Vertexes[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
		Button1Vertexes[3].texture = XMFLOAT2(0.0f, 0.0f);

		Button1Vertexes[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
		Button1Vertexes[4].texture = XMFLOAT2(1.0f, 0.0f);

		Button1Vertexes[5].position = XMFLOAT3(right, top - _Width, 0.0f);  // Bottom right.
		Button1Vertexes[5].texture = XMFLOAT2(1.0f, 1.0f);

		// создание нижней кнопки 
		Button2Vertexes[0].position = XMFLOAT3(left, bottom + _Width, 0.0f);  // Top left.
		Button2Vertexes[0].texture = XMFLOAT2(1.0f, 1.0f);

		Button2Vertexes[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
		Button2Vertexes[1].texture = XMFLOAT2(0.0f, 0.0f);

		Button2Vertexes[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
		Button2Vertexes[2].texture = XMFLOAT2(1.0f, 0.0f);

		// Second triangle.
		Button2Vertexes[3].position = XMFLOAT3(left, bottom + _Width, 0.0f);  // Top left.
		Button2Vertexes[3].texture = XMFLOAT2(1.0f, 1.0f);

		Button2Vertexes[4].position = XMFLOAT3(right, bottom + _Width, 0.0f);  // Top right.
		Button2Vertexes[4].texture = XMFLOAT2(0.0f, 1.0f);

		Button2Vertexes[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
		Button2Vertexes[5].texture = XMFLOAT2(0.0f, 0.0f);

		// это я рассчитываю свобобное пространство в строке для бегунка
		int ChainlessBarArea = (int)(_Heigth - (_Width * 2));
		// а это рассчет самой длинны бегунка для программы
		//TravellerLenght = ChainlessBarArea / g_NumAlveola;
		if (TravellerLenght > 1)
			// а это для рисования
			DrawingTravellerLenght = TravellerLenght;
		else {
			DrawingTravellerLenght = 1;
			TravellerLenght = 1;
		}


		TravellerVertexes[0].position = XMFLOAT3(left, top - _Width, 0.0f);  // Top left.
		TravellerVertexes[0].texture = XMFLOAT2(0.0f, 0.0f);

		TravellerVertexes[1].position = XMFLOAT3(right, top - _Width - DrawingTravellerLenght, 0.0f);  // Bottom right.
		TravellerVertexes[1].texture = XMFLOAT2(1.0f, 1.0f);

		TravellerVertexes[2].position = XMFLOAT3(left, top - _Width - DrawingTravellerLenght, 0.0f);  // Bottom left.
		TravellerVertexes[2].texture = XMFLOAT2(0.0f, 1.0f);

		// Second triangle.
		TravellerVertexes[3].position = XMFLOAT3(left, top - _Width, 0.0f);  // Top left.
		TravellerVertexes[3].texture = XMFLOAT2(0.0f, 0.0f);

		TravellerVertexes[4].position = XMFLOAT3(right, top - _Width, 0.0f);  // Top right.
		TravellerVertexes[4].texture = XMFLOAT2(1.0f, 0.0f);

		TravellerVertexes[5].position = XMFLOAT3(right, top - _Width - DrawingTravellerLenght, 0.0f);  // Bottom right.
		TravellerVertexes[5].texture = XMFLOAT2(1.0f, 1.0f);

	}

	Obj->SetVertexBuffer(vertices, 6);
	Button1->SetVertexBuffer(Button1Vertexes, 6);
	Button2->SetVertexBuffer(Button2Vertexes, 6);
	Traveller->SetVertexBuffer(TravellerVertexes, 6);

	delete[] vertices;
	delete[] Button1Vertexes;
	delete[] Button2Vertexes;

	MidleTraveller = DrawingTravellerLenght / 2;


	// если задан пункт отображать кнопки
	if (IsButtonShow) {

		// эти нужны для глобальных кординат
		ChainlesBarTop = Top + RealPixWidth;
		ChainlessBarBottom = Bottom - RealPixWidth;
		ChainlessBarLeft = Left + RealPixHeigth;
		ChainlessBarRight = Right - RealPixHeigth;
		g_TravellerLeftInGlobalCord = ChainlessBarLeft;
		g_TravellerTopInGlobalCord = ChainlesBarTop;

		// а эти для кординат в системе DirectX
		ChainlesBarTop2 = (int)(top - _Width);
		TravellerTop = ChainlesBarTop2;
		ChainlessBarBottom2 = (int)(bottom + _Width);
		ChainlessBarLeft2 = (int)(left + _Heigth);
		TravellerLeft = ChainlessBarLeft2;
		ChainlessBarRight2 = (int)(right - _Heigth);
	}
	// если задан пункт неотображать кнопки
	else {
		ChainlesBarTop = Top;
		ChainlessBarBottom = Bottom;
		ChainlessBarLeft = Left;
		ChainlessBarRight = Right;

		// а эти для кординат в системе DirectX
		ChainlesBarTop2 = (int)top;
		TravellerTop = ChainlesBarTop2;
		ChainlessBarBottom2 = (int)bottom;
		ChainlessBarLeft2 = (int)left;
		TravellerLeft = ChainlessBarLeft2;
		ChainlessBarRight2 = (int)right;
	}

	*ReSetsControl = true;

	return S_OK;
}


HRESULT ScrollBarClass::Frame(DIMOUSESTATE2& MouseState) {

	POINT MouseNowPos;
	GetCursorPos(&MouseNowPos);


	bool texturefinalcontrol = false;

	if (IsSomethingReSet)
		texturefinalcontrol = false;
	else
		texturefinalcontrol = true;

	IsSomethingReSet = true;


	int MouseXInWindow = MouseNowPos.x - g_WindowPosX, MouseYInWindow = MouseNowPos.y - g_WindowPosY;
	int MouseYInDirectX = g_ScreenHeight / 2 - MouseYInWindow,
		MouseXInDirectX = (g_ScreenWidth / 2)* -1 + MouseXInWindow;

	if (IsMouseDownOnObjec == false && MouseState.rgbButtons[0])
		return S_OK;

	IsMouseDownOnObjec = true;
	if (MouseState.rgbButtons[0])
		int a = true;

	if (((MouseNowPos.x > g_WindowPosX + Left && MouseNowPos.x < g_WindowPosX + Right) && (MouseNowPos.y > g_WindowPosY + Top && MouseNowPos.y < g_WindowPosY + Bottom)) && (MouseState.rgbButtons[0]))
		IsMouseDownOnObjec = true;
	else {
		if (MouseState.rgbButtons[0])
			IsMouseDownOnObjec = false;
	}

	// если строка активна
	if (Enalbled) {
		// если мышь на строке
		if ((MouseNowPos.x > g_WindowPosX + Left && MouseNowPos.x < g_WindowPosX + Right)
			&& (MouseNowPos.y > g_WindowPosY + Top && MouseNowPos.y < g_WindowPosY + Bottom)) {


			// Calculate the screen coordinates of the bottom of the bitmap.
			bottom = top - (float)_Heigth;

			bool  IsTextureReset = false;

			// если нажата левоя кнопка мыши
			if (MouseState.rgbButtons[0]) {
				*ReSetsControl = true;

				// если строка стоит вертикально
				if (g_HorOrVer < 0) {
					// если мышь на свободном простанстве строки
					if (MouseYInWindow > ChainlesBarTop + MidleTraveller - 1 && MouseYInWindow < ChainlessBarBottom - MidleTraveller + 1) {
						TravellerTop = (g_ScreenHeight / 2) - MouseYInWindow + MidleTraveller;
						TravellerTop = (int)(((float)TravellerTop / g_ScreenHeight) * startScreenHeigth);
						g_TravellerTopInGlobalCord = MouseYInWindow - MidleTraveller;
						// если проинитена текстура нажатого бегунка
						if (IsTravellerPressTexture != nullptr) {
							Traveller->SetObjectTexture(IsTravellerPressTexture);
							IsTextureReset = true;
						}

					}
					else {
						// если нажата верхняя кнопочка
						if (MouseYInWindow < ChainlesBarTop - g_Indention) {
							static int FrameControler = 0;

							// если прошло нужное количество файлов и если он не вышел за зону
							if ((FrameControler > 10) && (TravellerTop < ChainlesBarTop2)) {
								++TravellerTop;
								++g_TravellerTopInGlobalCord;
								FrameControler = 0;
							}
							++FrameControler;
							// если проинциализирована текстура нажатой кнопки
							if (IsButtonPressTexture != nullptr) {
								Button1->SetObjectTexture(IsButtonPressTexture);
								IsTextureReset = true;
							}

						}

						// если нажато пространство между полностью свободний частью строки и кнопкой
						if (MouseYInWindow > ChainlesBarTop - g_Indention &&  MouseYInWindow < ChainlesBarTop + MidleTraveller + g_Indention) {
							TravellerTop = ChainlesBarTop2;
							g_TravellerTopInGlobalCord = ChainlesBarTop;
							Button1->SetObjectTexture(OsnButtonsTexture);
							Button2->SetObjectTexture(OsnButtonsTexture);
							// если проинитена текстура нажатого бегунка
							if (IsTravellerPressTexture != nullptr) {
								Traveller->SetObjectTexture(IsTravellerPressTexture);
								IsTextureReset = true;
							}
						}

						// если нажата нижняя кнопочка
						if (MouseYInWindow > ChainlessBarBottom + g_Indention) {
							static int FrameControler = 0;

							// если прошло нужное количество файлов и если он не вышел за зону
							if ((FrameControler > 10) && (TravellerTop - DrawingTravellerLenght  > ChainlessBarBottom2)) {
								--TravellerTop;
								--g_TravellerTopInGlobalCord;
								FrameControler = 0;
							}
							++FrameControler;
							// если проинциализирована текстура нажатой кнопки
							if (IsButtonPressTexture != nullptr) {
								Button2->SetObjectTexture(IsButtonPressTexture);
								IsTextureReset = true;
							}

						}

						// если нажато пространство между полностью свободний частью строки и кнопкой
						if (MouseYInWindow < ChainlessBarBottom + g_Indention &&  MouseYInWindow > ChainlessBarBottom - MidleTraveller - g_Indention) {
							TravellerTop = ChainlessBarBottom2 + DrawingTravellerLenght;
							g_TravellerTopInGlobalCord = ChainlessBarBottom + DrawingTravellerLenght;
							Button1->SetObjectTexture(OsnButtonsTexture);
							Button2->SetObjectTexture(OsnButtonsTexture);
							// если проинитена текстура нажатого бегунка
							if (IsTravellerPressTexture != nullptr) {
								Traveller->SetObjectTexture(IsTravellerPressTexture);
								IsTextureReset = true;
							}
						}

					}

					TravellerVertexes[0].position = XMFLOAT3(left, (float)TravellerTop, 0.0f);  // Top left.
					TravellerVertexes[0].texture = XMFLOAT2(0.0f, 0.0f);

					TravellerVertexes[1].position = XMFLOAT3(right, (float)(TravellerTop - DrawingTravellerLenght), 0.0f);  // Bottom right.
					TravellerVertexes[1].texture = XMFLOAT2(1.0f, 1.0f);

					TravellerVertexes[2].position = XMFLOAT3(left, (float)(TravellerTop - DrawingTravellerLenght), 0.0f);  // Bottom left.
					TravellerVertexes[2].texture = XMFLOAT2(0.0f, 1.0f);

					// Second triangle.
					TravellerVertexes[3].position = XMFLOAT3(left, (float)TravellerTop, 0.0f);  // Top left.
					TravellerVertexes[3].texture = XMFLOAT2(0.0f, 0.0f);

					TravellerVertexes[4].position = XMFLOAT3(right, (float)TravellerTop, 0.0f);  // Top right.
					TravellerVertexes[4].texture = XMFLOAT2(1.0f, 0.0f);

					TravellerVertexes[5].position = XMFLOAT3(right, (float)(TravellerTop - DrawingTravellerLenght), 0.0f);  // Bottom right.
					TravellerVertexes[5].texture = XMFLOAT2(1.0f, 1.0f);

					Traveller->SetVertexBuffer(TravellerVertexes, 6);

					if (IsTextureReset)
						return S_OK;

				}
				// если горизонтально
				else {


					// если мышь на свободном простанстве строки
					if (MouseXInWindow > ChainlessBarLeft + MidleTraveller - 1 && MouseXInWindow < ChainlessBarRight - MidleTraveller + 1) {
						TravellerLeft = (g_ScreenWidth / 2) * -1 + MouseXInWindow - MidleTraveller;
						TravellerLeft = (int)(((float)TravellerLeft / g_ScreenWidth) * startScreenWidth);
						g_TravellerLeftInGlobalCord = MouseXInWindow - MidleTraveller;
						// если проинитена текстура нажатого бегунка
						if (IsTravellerPressTexture != nullptr) {
							Traveller->SetObjectTexture(IsTravellerPressTexture);
							IsTextureReset = true;
						}

					}
					else {
						// если нажата левоя кнопочка
						if (MouseXInWindow < ChainlessBarLeft - g_Indention) {
							static int FrameControler = 0;

							// если прошло нужное количество файлов и если он не вышел за зону
							if ((FrameControler > 10) && (TravellerLeft > ChainlessBarLeft2)) {
								--TravellerLeft;
								--g_TravellerLeftInGlobalCord;
								FrameControler = 0;
							}
							++FrameControler;
							// если проинциализирована текстура нажатой кнопки
							if (IsButtonPressTexture != nullptr) {
								Button1->SetObjectTexture(IsButtonPressTexture);
								IsTextureReset = true;
							}

						}

						// если нажато пространство между полностью свободний частью строки и кнопкой
						if (MouseXInWindow > ChainlessBarLeft - g_Indention &&  MouseXInWindow < ChainlessBarLeft + MidleTraveller) {
							TravellerLeft = ChainlessBarLeft2;
							g_TravellerLeftInGlobalCord = ChainlessBarLeft;
							Button1->SetObjectTexture(OsnButtonsTexture);
							Button2->SetObjectTexture(OsnButtonsTexture);
							// если проинитена текстура нажатого бегунка
							if (IsTravellerPressTexture != nullptr) {
								Traveller->SetObjectTexture(IsTravellerPressTexture);
								IsTextureReset = true;
							}
						}

						// если нажата правоя кнопочка
						if (MouseXInWindow > ChainlessBarRight + g_Indention) {
							static int FrameControler = 0;

							// если прошло нужное количество файлов и если он не вышел за зону
							if ((FrameControler > 10) && (TravellerLeft + DrawingTravellerLenght  < ChainlessBarRight2)) {
								++TravellerLeft;
								++g_TravellerLeftInGlobalCord;
								FrameControler = 0;
							}
							++FrameControler;
							// если проинциализирована текстура нажатой кнопки
							if (IsButtonPressTexture != nullptr) {
								Button2->SetObjectTexture(IsButtonPressTexture);
								IsTextureReset = true;
							}

						}

						// если нажато пространство между полностью свободний частью строки и кнопкой
						if (MouseXInWindow < ChainlessBarRight + g_Indention &&  MouseXInWindow > ChainlessBarRight - MidleTraveller) {
							TravellerLeft = ChainlessBarRight2 - DrawingTravellerLenght;
							g_TravellerLeftInGlobalCord = ChainlessBarRight - DrawingTravellerLenght;
							Button1->SetObjectTexture(OsnButtonsTexture);
							Button2->SetObjectTexture(OsnButtonsTexture);
							// если проинитена текстура нажатого бегунка
							if (IsTravellerPressTexture != nullptr) {
								Traveller->SetObjectTexture(IsTravellerPressTexture);
								IsTextureReset = true;
							}
						}

					}

					TravellerVertexes[0].position = XMFLOAT3((float)TravellerLeft, top, 0.0f);  // Top left.
					TravellerVertexes[0].texture = XMFLOAT2(0.0f, 0.0f);

					TravellerVertexes[1].position = XMFLOAT3((float)(TravellerLeft + DrawingTravellerLenght), bottom, 0.0f);  // Bottom right.
					TravellerVertexes[1].texture = XMFLOAT2(1.0f, 1.0f);

					TravellerVertexes[2].position = XMFLOAT3((float)TravellerLeft, bottom, 0.0f);  // Bottom left.
					TravellerVertexes[2].texture = XMFLOAT2(0.0f, 1.0f);

					// Second triangle.
					TravellerVertexes[3].position = XMFLOAT3((float)TravellerLeft, top, 0.0f);  // Top left.
					TravellerVertexes[3].texture = XMFLOAT2(0.0f, 0.0f);

					TravellerVertexes[4].position = XMFLOAT3((float)(TravellerLeft + DrawingTravellerLenght), top, 0.0f);  // Top right.
					TravellerVertexes[4].texture = XMFLOAT2(1.0f, 0.0f);

					TravellerVertexes[5].position = XMFLOAT3((float)(TravellerLeft + DrawingTravellerLenght), bottom, 0.0f);  // Bottom right.
					TravellerVertexes[5].texture = XMFLOAT2(1.0f, 1.0f);

					Traveller->SetVertexBuffer(TravellerVertexes, 6);


					if (IsTextureReset)
						return S_OK;

				}

			}
			// если ненажата левоя кнопка мыши
			else {

				// если строка стоит вертикально
				if (g_HorOrVer < 0) {
					// если мышь на верхней кнопочке    и проинициализирована текстура когда мышь на кнопке
					if (MouseYInWindow < ChainlesBarTop + g_Indention && IsMouseOnButtonTexture != nullptr) {
						Button1->SetObjectTexture(IsMouseOnButtonTexture);
						Traveller->SetObjectTexture(OsnTravellerTexture);
						IsSomethingReSet = false;
						openDraw(texturefinalcontrol);
						return S_OK;
					}

					// если мышь на нижней кнопочке
					if (MouseYInWindow > ChainlessBarBottom - g_Indention && IsMouseOnButtonTexture != nullptr) {
						Button2->SetObjectTexture(IsMouseOnButtonTexture);
						Traveller->SetObjectTexture(OsnTravellerTexture);
						IsSomethingReSet = false;
						openDraw(texturefinalcontrol);
						return S_OK;
					}

					// если мышь на бегуке и проинитена текстура когда мышь на бегунке
					if ((MouseYInDirectX < TravellerTop && MouseYInDirectX > TravellerTop - DrawingTravellerLenght) && IsMouseOnTravellerTexture != NULL) {
						Traveller->SetObjectTexture(IsMouseOnTravellerTexture);
						Button1->SetObjectTexture(OsnButtonsTexture);
						Button2->SetObjectTexture(OsnButtonsTexture);
						IsSomethingReSet = false;
						openDraw(texturefinalcontrol);
						return S_OK;
					}
				}
				// если горизонтально
				else {
					// если мышь на левой кнопочке
					if (MouseXInWindow < ChainlessBarLeft - g_Indention && IsMouseOnButtonTexture != nullptr) {
						Button1->SetObjectTexture(IsMouseOnButtonTexture);
						Traveller->SetObjectTexture(OsnTravellerTexture);
						IsSomethingReSet = false;
						openDraw(texturefinalcontrol);
						return S_OK;
					}

					// если мышь на правой кнопочке
					if (MouseXInWindow > ChainlessBarRight + g_Indention && IsMouseOnButtonTexture != nullptr) {
						Button2->SetObjectTexture(IsMouseOnButtonTexture);
						Traveller->SetObjectTexture(OsnTravellerTexture);
						IsSomethingReSet = false;
						openDraw(texturefinalcontrol);
						return S_OK;
					}

					// если мышь на бегуке и проинитена текстура когда мышь на бегунке
					if ((MouseXInDirectX > TravellerLeft && MouseXInDirectX < TravellerLeft + DrawingTravellerLenght) && IsMouseOnTravellerTexture != NULL) {
						Traveller->SetObjectTexture(IsMouseOnTravellerTexture);
						Button1->SetObjectTexture(OsnButtonsTexture);
						Button2->SetObjectTexture(OsnButtonsTexture);
						IsSomethingReSet = false;
						openDraw(texturefinalcontrol);
						return S_OK;
					}

				}

			}
		}

		Button1->SetObjectTexture(OsnButtonsTexture);
		Button2->SetObjectTexture(OsnButtonsTexture);
		Traveller->SetObjectTexture(OsnTravellerTexture);
		Obj->SetObjectTexture(OsnBarTexture);

	}
	// если строка не активна
	else {
		IsSomethingReSet = false;
		// если текстура неактивной кнопки проинитена
		if (IsBarNotEnalbledButtonsTexture != nullptr) {
			Button1->SetObjectTexture(IsBarNotEnalbledButtonsTexture);
			Button2->SetObjectTexture(IsBarNotEnalbledButtonsTexture);
		}
		// если проинитена текстура неоктивного бегунка
		if (IsNotEnalbledTravellerTexture != nullptr)
			Traveller->SetObjectTexture(IsNotEnalbledTravellerTexture);
		// если проинитена текстура неоктивной строки
		if (NotEnalbledBarTexture != nullptr)
			Obj->SetObjectTexture(NotEnalbledBarTexture);

	}

	openDraw(texturefinalcontrol);

	return S_OK;

}


void ScrollBarClass::ResetObjReadingState(int left, int rigth, int top, int bottom) {

	Top = top;
	Bottom = bottom;
	Right = rigth;
	Left = left;

	// если задан пункт отображать кнопки
	if (IsButtonShow) {

		RealPixWidth = rigth - left;
		RealPixHeigth = bottom - top;

		// эти нужны для глобальных кординат 
		ChainlesBarTop = Top + RealPixWidth;
		ChainlessBarBottom = Bottom - RealPixWidth;
		ChainlessBarLeft = Left + RealPixHeigth;
		ChainlessBarRight = Right - RealPixHeigth;
		g_TravellerLeftInGlobalCord = ChainlessBarLeft;
		g_TravellerTopInGlobalCord = ChainlesBarTop;

	}
	// если задан пункт неотображать кнопки
	else {
		ChainlesBarTop = Top;
		ChainlessBarBottom = Bottom;
		ChainlessBarLeft = Left;
		ChainlessBarRight = Right;

	}

}


void ScrollBarClass::ResetWindowParam(int WindowPosX, int WindowPosY, int WindowWidth, int WindowHeigth) {

	g_WindowPosX = WindowPosX;
	g_WindowPosY = WindowPosY;
	g_ScreenWidth = WindowWidth;
	g_ScreenHeight = WindowHeigth;

	SetPos();

}
#include "stdafx.h"
#include "KFTextureOnTextureDrawing.h"

KFTextureOnTextureDrawing::KFTextureOnTextureDrawing(){

	IsSomeStingreset = true;
	m_renderTargetTexture = nullptr;
	m_renderTargetView = nullptr;
	m_shaderResourceView = nullptr;
	osnBackGroun = nullptr;
	Obj = nullptr;

}

KFTextureOnTextureDrawing::~KFTextureOnTextureDrawing(){

	int c = 0;
	while (c < ObjsArray.size()){

		delete ObjsArray[c];

		++c;
	}

	if (Obj != nullptr){

		delete Obj;

	}

	if (osnBackGroun != nullptr){

		delete osnBackGroun;

	}

	if (m_shaderResourceView != nullptr)
	{
		m_shaderResourceView->Release();
		m_shaderResourceView = 0;
	}

	if (m_renderTargetView != nullptr)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_renderTargetTexture != nullptr)
	{
		m_renderTargetTexture->Release();
		m_renderTargetTexture = 0;
	}

}

HRESULT KFTextureOnTextureDrawing::Init(HWND hwnd, D3DGlobalContext* D3DGC, UINT StTextureWidth, UINT StTextureHeigth, 
	int screenPosX, int ScreenPosY, ID3D11ShaderResourceView * BackGroundTexture,
	ID3D11RenderTargetView * ActiveRenderTargetView, float PosX, float PosY,
	ID3D11DepthStencilView * ActiveDepthStensil, ID3D10Blob * WorkBlob){

	m_ActiveDepthStensil = ActiveDepthStensil;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	g_deviceContext = D3DGC->D11_deviceContext;
	g_ActiveRenderTargetView = ActiveRenderTargetView;

	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = (UINT)StTextureWidth;
	textureDesc.Height = (UINT)StTextureHeigth;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = D3DGC->D11_device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"KFTextureManegerClass. ошибка в создании m_renderTargetTexture", L"Error", MB_OK);
		return E_FAIL;
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = D3DGC->D11_device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"KFTextureManegerClass. ошибка в создании рендер таргета", L"Error", MB_OK);
		return E_FAIL;
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = D3DGC->D11_device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"KFTextureManegerClass. ошибка в создании шейдер ресурсов", L"Error", MB_OK);
		return E_FAIL;
	}

	osnBackGroun = new SquareObjectClass;

	XMFLOAT4 ObjData = { 0.0f, 0.0f, (float)StTextureWidth,	(float)StTextureHeigth };
	
	osnBackGroun->Init(D3DGC, ObjData, BackGroundTexture, 0, WorkBlob);

	ObjData = { PosX * D3DGC->ScreenWidth, PosY * D3DGC->ScreenHeight, (float)StTextureWidth, (float)StTextureHeigth };
	
	Obj = new SquareObjectClass;
	Obj->Init(D3DGC, ObjData, m_shaderResourceView, 0, WorkBlob);

	OsnTextureWidth = StTextureWidth;
	OsnTextureHeigth = StTextureHeigth;
	g_hwnd = hwnd;
	g_ScreenWidth = D3DGC->ScreenWidth;
	g_ScreenHeigth = D3DGC->ScreenHeight;
	g_ScreenX = screenPosX;
	g_ScreenY = ScreenPosY;
	g_device = D3DGC->D11_device;
	g_DevCon = D3DGC->D11_deviceContext;


	return S_OK;
}

void KFTextureOnTextureDrawing::DrawTextres(){


	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = 0.0f;
	color[1] = 0.0f;
	color[2] = 0.0f;
	color[3] = 0.0f;

	g_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, NULL);

	// Clear the back buffer.
	g_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	osnBackGroun->Draw();

	int c = 0;
	while (c < ObjsArray.size()){

		ObjsArray[c]->Draw();
		++c;

	}

	g_deviceContext->OMSetRenderTargets(1, &g_ActiveRenderTargetView, m_ActiveDepthStensil);

}

void KFTextureOnTextureDrawing::Draw(){

	if (IsSomeStingreset){
		DrawTextres();
		IsSomeStingreset = false;
	}

	Obj->Draw();

}

void KFTextureOnTextureDrawing::AddTexture(D3DGlobalContext* D3DGC, float TexPosX , float TexPosY , float texWidth , float texHeigth
	, ID3D11ShaderResourceView * Texture , ID3D10Blob * WorkBlob){

	XMFLOAT4 ObjData = { TexPosX * OsnTextureWidth
		, TexPosY * OsnTextureHeigth, texWidth, texHeigth };

	SquareObjectClass * TimeObj;
	TimeObj = new SquareObjectClass;
	TimeObj->Init(D3DGC, ObjData, Texture, 0, WorkBlob);

	ObjsArray.push_back(TimeObj);

}
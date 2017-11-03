#include "stdafx.h"
#include "KFShadowWork.h"

KFShadowWork::KFShadowWork() {

	cbShadowBuffer = nullptr;
	LightRender_RS = nullptr;
	LightRender_DS = nullptr;
	DSV_ShadowMap3D = nullptr;
	Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	LightPosition.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };
	LightTarget.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };
}

KFShadowWork::~KFShadowWork() {

	RCUBE_RELEASE(cbShadowBuffer);
	RCUBE_RELEASE(LightRender_RS);
	RCUBE_RELEASE(LightRender_DS);

}

void KFShadowWork::Init(HWND hwnd, D3DGlobalContext * D3DGC, D3DClass * Light, FrustumClass* Frustum
	, ObjectUsingShadows  Objects) {

	DrawShadowsObjects = Objects;
	g_Frustum = Frustum;
	g_Light = Light;
	Local_D3DGC = D3DGC;
	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbShadowObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	HRESULT hr = Local_D3DGC->DX_device->CreateBuffer(&cbbd, NULL, &cbShadowBuffer);

	// ДЛЯ ТЕНИ
	// Для отладки вынес в отдельную функцию
	InitRasterizerState( 1, 1.0f );


	{
		CD3D11_DEPTH_STENCIL_DESC desc(D3D11_DEFAULT);
		// NOTE: Complementary Z => GREATER test
		desc.DepthEnable = true;
		desc.StencilEnable = false;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;//D3D11_COMPARISON_GREATER;//D3D11_COMPARISON_LESS;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		Local_D3DGC->DX_device->CreateDepthStencilState(&desc, &LightRender_DS);
	}

	ZeroMemory(&DSD2, sizeof(DSD2));
	DSD2.Flags = 0;
	DSD2.Format = DXGI_FORMAT_D32_FLOAT;
	if (Local_D3DGC->MSAAQualityCount > 1)
	{
		DSD2.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
		DSD2.Texture2DMSArray.ArraySize = 1;
		DSD2.Texture2DMSArray.FirstArraySlice = 0;
	}
	else
	{
		DSD2.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		DSD2.Texture2DArray.MipSlice = 0;
		DSD2.Texture2DArray.ArraySize = 1;
		DSD2.Texture2DArray.FirstArraySlice = 0;
	}

}

void KFShadowWork::RenderSpotLightsSadowMaps(std::vector <int> SpotLightsWithShadowsIndexes) {

	HRESULT res;

	cbShadowObject cbPerObj;


	ID3D11ShaderResourceView * tab[1];
	tab[0] = NULL;
	Local_D3DGC->DX_deviceContext->PSSetShaderResources(1, 1, tab);
	Local_D3DGC->DX_deviceContext->PSSetShader(0, 0, 0);

//	Local_D3DGC->DX_deviceContext->VSSetShader(myManeger->VertShaderArr[ShaderFordraw], 0, 0);
	Local_D3DGC->DX_deviceContext->RSSetState(LightRender_RS);
	Local_D3DGC->DX_deviceContext->GSSetShader(0, 0, 0);
	Local_D3DGC->DX_deviceContext->OMSetDepthStencilState(LightRender_DS, 0);
	// Устанавливаем ViewPort для ShadowMap
	Local_D3DGC->DX_deviceContext->RSSetViewports(1, &g_Light->LightViewPort);
//	g_D3DGC->DX_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	int Amount = (int)SpotLightsWithShadowsIndexes.size();
	for (int i = 0; i < Amount; i++)
	{

		// Для скорости убрал всё что не меняется в глобальную инициализацию
		if (Local_D3DGC->MSAAQualityCount > 1)
		{
			DSD2.Texture2DMSArray.FirstArraySlice = i;
		}
		else
		{
			DSD2.Texture2DArray.FirstArraySlice = i;
		}

		res = Local_D3DGC->DX_device->CreateDepthStencilView(g_Light->ShadowMap3D, &DSD2, &DSV_ShadowMap3D);
		//if (FAILED(res))
		//{
		//	return false;
		//}
#if defined( DEBUG ) || defined( _DEBUG )
		const char c_szName[] = "DSV_ShadowMap3D";
		DSV_ShadowMap3D->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(c_szName) - 1, c_szName);
#endif
		// Для скорости создаём указатель
		int* Point = &SpotLightsWithShadowsIndexes[i];
		PointLight* Point1 = g_Light->mPointLightParameters[*Point];

			LightPosition.Fl3 = Point1->position;
//			LightPosition.Fl4.w = 0.0f;
			LightTarget.Fl3 = Point1->direction;
//			LightTarget.Fl4.w = 0.0f;
//			LightTarget.Vec = LightTarget.Vec / 100.0f;
//			XMVECTOR LightTarget = XMLoadFloat3(&g_Light->mPointLightParameters[*Point]->direction);
// http://www.3dgep.com/understanding-the-view-matrix/
// http://www.codinglabs.net/article_world_view_projection_matrix.aspx
		XMMATRIX LightView = XMMatrixLookToLH(LightPosition.Vec, LightTarget.Vec, Up); //XMVector3Normalize  / XMVector3NormalizeEst

		XMMATRIX LightProjection = XMMatrixPerspectiveFovLH(XM_PIDIV4, Local_D3DGC->ScreenRatio,
			Local_D3DGC->NearPlane, Point1->attenuationEnd); //m_Light->D3DGC_Light->FarPlane * 10
//		LightProjection = XMMatrixOrthographicLH( g_D3DGC->ScreenWidth, g_D3DGC->ScreenHeight, g_D3DGC->NearPlane, Point1->attenuationEnd );

		XMMATRIX LightViewProj = LightView * LightProjection;
		// Это пока не нужно.  У нас пока нет отсеивание объектов по светам. Это нужно не здесь.
		g_Frustum->ConstructFrustumNew(LightViewProj);

		cbPerObj.ViewProjection = XMMatrixTranspose(LightViewProj);

		Local_D3DGC->DX_deviceContext->UpdateSubresource(cbShadowBuffer, 0, NULL, &cbPerObj, 0, 0);
		Local_D3DGC->DX_deviceContext->VSSetConstantBuffers(1, 1, &cbShadowBuffer);
		// Создаём матрицу поворота
		RCubeMatrix Mat;
		Mat.XMM = cbPerObj.ViewProjection;
		Point1->qtwvp = Mat.XMF;
//		XMStoreFloat4x4(&Point1->qtwvp, cbPerObj.ViewProjection );
// ----------------------------------------------------------------------------------
		// Создаём кватернион поворота
//		LightTarget.Vec = XMQuaternionRotationMatrix( cbPerObj.ViewProjection );
//		Point1->RotQuat = LightTarget.Fl4;

//		cbPerObj.ViewProjection = XMMatrixRotationQuaternion(LightTarget.Vec);
// ----------------------------------------------------------------------------------
		Local_D3DGC->DX_deviceContext->OMSetRenderTargets(0, 0, DSV_ShadowMap3D);
		Local_D3DGC->DX_deviceContext->ClearDepthStencilView(DSV_ShadowMap3D, D3D11_CLEAR_DEPTH, 1.0f, 0);
		DrawObjectUsingShadows(LightPosition.Vec, true);

		// Сохраняем в световом массиве индекс куска Shadow Map
		Point1->ShadowMapSliceNumber = i;
		Point1->LightID = i;
		DSV_ShadowMap3D->Release();

	}
	// Устанавливаем ViewPort для RCube
	Local_D3DGC->DX_deviceContext->RSSetViewports(1, &g_Light->viewport);
}

void KFShadowWork::DrawObjectUsingShadows(XMVECTOR DrawPosition, bool ReplaseData) {

//	DrawShadowsObjects.Terrain->Frame(ReplaseData, DrawPosition);
	DrawShadowsObjects.Terrain->Render();

	DrawShadowsObjects.ModelList->Frame();
	DrawShadowsObjects.ModelList->Render();

}


void KFShadowWork::InitRasterizerState( int DepthBias, float SlopeScaledDepthBias )
{
	RCUBE_RELEASE(LightRender_RS );
	CD3D11_RASTERIZER_DESC desc( D3D11_DEFAULT );
	desc.CullMode = D3D11_CULL_FRONT; //D3D11_CULL_BACK; //D3D11_CULL_FRONT; //D3D11_CULL_NONE;//
	desc.FillMode = D3D11_FILL_SOLID;//D3D11_FILL_WIREFRAME;//D3D11_FILL_SOLID;//
	desc.FrontCounterClockwise = false;
	desc.ScissorEnable = false;
	desc.DepthClipEnable = true;//true;
	desc.MultisampleEnable = true;//false;
	desc.AntialiasedLineEnable = true;
	desc.DepthBias = DepthBias;//1.e5;//0.0f;//1.e5;
	desc.SlopeScaledDepthBias = SlopeScaledDepthBias;// 1.0f;//8.0;//0.0f;//8.0;
	desc.DepthBiasClamp = 1.0f;
	Local_D3DGC->DX_device->CreateRasterizerState( &desc, &LightRender_RS );
}
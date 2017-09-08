#include "stdafx.h"
////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "lightclass.h"


LightClass::LightClass()
{
			  Light = nullptr;

	 lightBufferSRV = nullptr;
 LightGridBufferSRV = nullptr;

		ShadowMap3D = nullptr;
	SRV_ShadowMap3D = nullptr;
	
	 PointLightSize = sizeof( PointLight );

 SMWidth = 512; // Ширина Shadow Map
 SMHeight = 512;// Высота Shadow Map
 
 mPointLightParameters.reserve( MAX_LIGHTS );

 FreeLightIndex.reserve( MAX_LIGHTS );

 SpotLightsWithShadowsIndexes.reserve( MAX_LIGHTS );

	   LightPos.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };
   LightPosCopy.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };
		   Temp.Vec = { 0.0f, 0.0f, 0.0f, 1.0f }; 
		  Temp2.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };
 				One = { 1.0f, 1.0f, 1.0f, 1.0f };
			HalfOne = { 1.0f, 1.0f, 0.0f, 0.0f };
		clipMinFast = XMFLOAT2(-1.0f, -1.0f);
		clipMaxFast = XMFLOAT2(1.0f, 1.0f);

	 ClustersAmount = 0;

mLightGridBufferSize = 64 * 1024 * 32; // 32 MB;

// mLightGridBuilder.reset( LightGridDimensions( 32, 32, 128 ) );

memset( &TempL, 0, PointLightSize );
TempL.ShadowMapSliceNumber = -5;
TempL.Dummy = -1;
}


LightClass::~LightClass()
{
}


void LightClass::Shutdown()
{
	DeleteAllLights();

	RCUBE_DELETE( Light );
	RCUBE_DELETE( mLightGridBuffer );
	RCUBE_RELEASE( mPerFrameConstants );
}


/*
XMFLOAT3 operator* (const XMFLOAT3& l, const float r) {
	XMVECTOR lvec(XMLoadFloat3(&l));
	XMFLOAT3 Res;
	XMStoreFloat3(&Res, lvec * r);
	return Res;
}
*/


template<typename T>
inline T clamp(T v, T lb, T ub)
{
	return min(max(v, lb), ub);
}


inline void LightClass::UpdateClipRegion(
	float lc,          // Light x/y coordinate (view space)
	float& lz,          // Light z coordinate (view space)
	float& lightRadius,
const float& cameraScale, // Project scale for coordinate (_11 or _22 for x/y respectively)
	float& clipMin,
	float& clipMax)
{
	float rSq = lightRadius * lightRadius;
	float lcSq = lc * lc;
	float lzSq = lz * lz;
	float lcSqPluslzSq = lcSq + lzSq;
	float d = rSq * lcSq - lcSqPluslzSq * (rSq - lzSq);

	if (d > 0)
	{
		float a = lightRadius * lc;
		float b = sqrtf(d);
		float nx0 = (a + b) / lcSqPluslzSq;
		float nx1 = (a - b) / lcSqPluslzSq;
		float lcSqPluslzSq_Minus_rSq = lcSqPluslzSq - rSq;
		// X
		//		UpdateClipRegionRoot(nx0, lc, lz, lcSqPluslzSq, lightRadius, cameraScale, clipMin, clipMax);
		float nz = (lightRadius - nx0 * lc) / lz;
		float pz = lcSqPluslzSq_Minus_rSq / (lz - (nz / nx0) * lc);
		if (pz > 0.0f) {
			float c = -nz * cameraScale / nx0;
			if (nx0 > 0.0f)
			{                      // Left side boundary
				clipMin = max(clipMin, c);
			}
			else
			{                       // Right side boundary
				clipMax = min(clipMax, c);
			}
		}
		// Y
		//		UpdateClipRegionRoot(nx1, lc, lz, lcSqPluslzSq, lightRadius, cameraScale, clipMin, clipMax);
		nz = (lightRadius - nx1 * lc) / lz;
		pz = lcSqPluslzSq_Minus_rSq / (lz - (nz / nx1) * lc);
		if (pz > 0.0f) {
			float c = -nz * cameraScale / nx1;
			if (nx1 > 0.0f)
			{                      // Left side boundary
				clipMin = max(clipMin, c);
			}
			else
			{                       // Right side boundary
				clipMax = min(clipMax, c);
			}
		}
	}
}


// Returns bounding box [min.xy, max.xy] in clip [-1, 1] space.
inline XMVECTOR LightClass::ComputeClipRegion(
	XMFLOAT3& lightPosView,
	float& lightRadius,
	const float& float11,
	const float& float22,
	const float& mCameraNearFarX)
{
	// Early out with empty rectangle if the light is too far behind the view frustum
//	XMVECTOR clipRegion = XMVECTOR{ 1.0f, 1.0f, 0.0f, 0.0f };
	XMVECTOR clipRegion = HalfOne;
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++       Отсеиваем свет невидимый на экране         ++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if ( lightPosView.z + lightRadius >= mCameraNearFarX )
	{
//		XMFLOAT2 clipMin = XMFLOAT2(-1.0f, -1.0f);
//		XMFLOAT2 clipMax = XMFLOAT2(1.0f, 1.0f);
		XMFLOAT2 clipMin = clipMinFast;
		XMFLOAT2 clipMax = clipMaxFast;

		UpdateClipRegion(lightPosView.x, lightPosView.z, lightRadius, float11, clipMin.x, clipMax.x);
		UpdateClipRegion(-lightPosView.y, lightPosView.z, lightRadius, float22, clipMin.y, clipMax.y);

		clipRegion = XMVECTOR{ clipMin.x, clipMin.y, clipMax.x, clipMax.y };
	}

	return clipRegion;
}


inline void LightClass::GenerateLightFragments(
	const FragmentFactory& fragmentFactory,
	LightGridBuilder* builder,
	XMMATRIX& ProjMatrix,
	XMMATRIX& CameraMatrix,
	PointLight* light,
	int lightIndex)
{

	LightGridDimensions dim = builder->dimensions();
	RCube_VecFloat34 mCameraNearFar;
	RCubeMatrix MyMatrix;
//	XMFLOAT4X4 mCameraProj;

//	XMStoreFloat4x4(&mCameraProj, ProjMatrix);
	MyMatrix.XMM = ProjMatrix;

	RCube_VecFloat34 clipRegion1, clipRegion, position, direction;

	mCameraNearFar.Vec = Light->mCameraNearFar;

	float attEnd;

	position.Fl3 = light->position;
	direction.Fl3 = light->direction;

//	clipRegion1.Vec = XMVECTOR{ 1.0f, 1.0f, 0.0f, 0.0f };
	// Преобразовываем координаты центра источника света в соответствии откуда на него смотрим, чтобы правильно 
	// расчитать кластера занимаемые светом на экране
/*	if (light->angel <= 80.0f && light->angel != 0.0f)
	{
		attEnd = light->attenuationEnd / 1.58f;//2;
		LightPos.Vec = (XMVector3Normalize(direction.Vec) * attEnd + position.Vec);
	}
	if (light->angel == 0.0f || light->angel > 80.0f)
*/	{
		LightPos.Fl3 = light->position;
		attEnd = light->attenuationEnd;
	}

//	LightPos.Fl3 = light->position;
	LightPosCopy.Vec = XMVector3TransformCoord(LightPos.Vec, CameraMatrix); //camView WorldPos

	clipRegion1.Vec = ComputeClipRegion(LightPosCopy.Fl3, attEnd, MyMatrix.XMF._11, MyMatrix.XMF._22, mCameraNearFar.Fl4.x);
//	clipRegion1.Vec = ComputeClipRegion(LightPosCopy.Fl3, attEnd, mCameraProj._11, mCameraProj._22, mCameraNearFar.Fl4.x);
//	clipRegion1.Vec = ComputeClipRegion(LightPosCopy.Fl3, light->attenuationEnd, mCameraProj._11, mCameraProj._22, mCameraNearFar.Fl4.x);
	// compute view space quad
//	clipRegion.Vec = (clipRegion1.Vec + XMVECTOR{ 1.0f, 1.0f, 1.0f, 1.0f }) / 2; // map coordinates to [0..1]
	clipRegion.Vec = (clipRegion1.Vec + One) / 2; // map coordinates to [0..1]

	int intClipRegion[4];

	intClipRegion[0] = (int)(clipRegion.Fl4.x * dim.width);
	intClipRegion[1] = (int)(clipRegion.Fl4.y * dim.height);
	intClipRegion[2] = (int)(clipRegion.Fl4.z * dim.width);
	intClipRegion[3] = (int)(clipRegion.Fl4.w * dim.height);

	if (intClipRegion[0] < 0) intClipRegion[0] = 0;
	if (intClipRegion[1] < 0) intClipRegion[1] = 0;
	if (intClipRegion[2] >= dim.width) intClipRegion[2] = dim.width - 1;
	if (intClipRegion[3] >= dim.height) intClipRegion[3] = dim.height - 1;

	float y_x = mCameraNearFar.Fl4.y - mCameraNearFar.Fl4.x;
	float center_z = (light->position.z - mCameraNearFar.Fl4.x) / y_x;
	float dist_z = light->attenuationEnd / y_x;

	int intZBounds[2];
	intZBounds[0] = (int)((center_z - dist_z)* dim.depth);
	intZBounds[1] = (int)((center_z + dist_z)* dim.depth);

	if (intZBounds[0] < 0) intZBounds[0] = 0;
	if (intZBounds[1] < 0) intZBounds[1] = 0;
	else if (intZBounds[1] >= dim.depth) intZBounds[1] = dim.depth - 1;


	for (int y = intClipRegion[1] / 4; y <= intClipRegion[3] / 4; ++y)
	{
		int y4 = y * 4;
		for (int x = intClipRegion[0] / 4; x <= intClipRegion[2] / 4; ++x)
		{
			int x4 = x * 4;
			for (int z = intZBounds[0] / 4; z <= intZBounds[1] / 4; ++z)
			{
				int z4 = z * 4;
				int x1 = clamp(intClipRegion[0] - x4, 0, 3);
				int x2 = clamp(intClipRegion[2] - x4, 0, 3);
				int y1 = clamp(intClipRegion[1] - y4, 0, 3);
				int y2 = clamp(intClipRegion[3] - y4, 0, 3);
				int z1 = clamp(intZBounds[0] - z4, 0, 3);
				int z2 = clamp(intZBounds[1] - z4, 0, 3);

				uint64_t coverage = fragmentFactory.coverage(x1, x2, y1, y2, z1, z2);

				builder->pushFragment(dim.cellIndex(x, y, z), lightIndex, coverage);
			}
		}
	}
}


ID3D11ShaderResourceView * LightClass::FrameLights(ID3D11DeviceContext* d3dDeviceContext, const UIConstants* ui)
{
	int n = ui->clusteredGridScale;

	if ( ClustersAmount != n )
	{
		mLightGridBuilder.reset ( LightGridDimensions ( 2 * n, n, 8 * n ) );

//		mLightGridBuilder.clearAllFragments ();
		//	FragmentFactory fragmentFactory;
		ClustersAmount = n;
	}
//	else
		mLightGridBuilder.clearAllFragments ();

// ???????????????????????????????????????????????????????????????????????????????????????????????????????
// ???????????????????????????????????????????????????????????????????????????????????????????????????????
// ???????????????????????????????????????????????????????????????????????????????????????????????????????
// ???????????????????????????????????????????????????????????????????????????????????????????????????????

	if (mActiveLights == 0)
		assert("Нет ни одного источника света в сцене");// return nullptr;
// ???????????????????????????????????????????????????????????????????????????????????????????????????????
// ???????????????????????????????????????????????????????????????????????????????????????????????????????
// ???????????????????????????????????????????????????????????????????????????????????????????????????????

	PointLight* light = mLightBuffer->MapDiscard(d3dDeviceContext);

	int c = 0;
	mVisibleLights = 0;

	PointLight *Dest = &light[0];
	PointLight **Source = &mPointLightParameters[0];

	// Обновляем массив источников света для шейдера.
	while (c < mActiveLights)
	{
		PointLight *Source1 = *Source;

		// Увеличиваем радиус свету, чтобы он не пропадал с экрана раньше времени
		float Radius = Source1->attenuationEnd;// * 1.5f;
		// Убираем свет из отрисовки, если он не виден на экране
		// если свет вообще нужно рисовать и по Frustum  
		if ( Source1->Dummy > -1 && Frustum->CheckSphere( Source1->position, Radius ) ) //
		{
/*			if ( !Frustum->CheckSphere( Source1->position, Radius ) )
			{
				if ( Source1->)
			}
			else
			{
*/
			GenerateLightFragments(fragmentFactory, &mLightGridBuilder, D3DGC_Light->ProjectionMatrix, D3DGC_Light->ViewMatrix, *Source, Source1->Dummy );
			// Copy light list into shader buffer
			memcpy( Dest, Source1, PointLightSize );
//			*Dest = *Source1;
//			}
			++mVisibleLights;
		}
		else
			Dest->Dummy = -1; // Источник света не виден в кадре
//			memcpy( Dest, &TempL, PointLightSize );

		++Source;
		++Dest;
		++c;
	}

	mLightBuffer->Unmap(d3dDeviceContext);

	LightGridEntry* gpuBuffer = mLightGridBuffer->MapDiscard(d3dDeviceContext);
	mLightGridBuilder.buildAndUpload(gpuBuffer/*, mLightGridBufferSize * 16*/);
	mLightGridBuffer->Unmap(d3dDeviceContext);

	LightGridBufferSRV = mLightGridBuffer->GetShaderResource();

	return mLightBuffer->GetShaderResource();
}


void LightClass::SetAmbientColor(XMFLOAT4& Color)
{
	Light->AmbientColor = Color;
}


void LightClass::SetDiffuseColor(XMFLOAT4& Color)
{
	Light->DiffuseColor = Color;
}


void LightClass::SetDiffusePower(float& Power)
{
	Light->DiffusePower = Power;
}


void LightClass::SetDiffuseDirection(XMFLOAT3& Direction)
{
	Light->DiffuseDirection = Direction;
}


void LightClass::GetDiffuseDirection(XMFLOAT3& Direction)
{
	Direction = Light->DiffuseDirection;
}


void LightClass::SetSpecularColor(XMFLOAT4& Color)
{
	Light->SpecularColor = Color;
}


void LightClass::SetSpecularPower(float& power)
{
//	Light->SpecularColor.w = power;
	Light->SpecularPower = power;
}


void LightClass::SetSpecularDirection(XMFLOAT3& Direction)
{
	Light->SpecularDirection = Direction;
}


void LightClass::GetSpecularDirection(XMFLOAT3& Direction)
{
	Direction = Light->SpecularDirection;
}


void LightClass::GetAmbientColor(XMFLOAT4& AmbColor)
{
	AmbColor = Light->AmbientColor;
}


void LightClass::GetDiffuseColor(XMFLOAT4 &DiffColor)
{
	DiffColor = Light->DiffuseColor;
}


void LightClass::GetSpecularColor(XMFLOAT4& SpecColor)
{
	SpecColor = Light->SpecularColor;
}


void LightClass::GetSpecularPower(float& SpecPower)
{
//	SpecPower = Light->SpecularColor.w;
	SpecPower = Light->SpecularPower;
}


float LightClass::GetSpecularPower()
{
	return Light->SpecularPower;
}


bool LightClass::Init( HWND hwnd, D3DGlobalContext* D3DGC, FrustumClass* frustum)
{
	Frustum = frustum;
	
	D3DGC_Light = D3DGC;

	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.Width = static_cast<float>(D3DGC_Light->ScreenWidth);
	viewport.Height = static_cast<float>(D3DGC_Light->ScreenHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
// Размер окна при отресовке теней
	ZeroMemory(&LightViewPort, sizeof(D3D11_VIEWPORT));
	LightViewPort.Width = static_cast<float>( 3192 );//D3DGC_Light->ScreenWidth ); // 512;//
	LightViewPort.Height = static_cast<float>( 3192 );//D3DGC_Light->ScreenHeight ); // 512;//
	LightViewPort.MinDepth = 0.0f;
	LightViewPort.MaxDepth = 1.0f;
	LightViewPort.TopLeftX = 0.0f;
	LightViewPort.TopLeftY = 0.0f;

	// Создаём константный буфер для базоого света Diffuse,Ambient,Specular
	// cb_LightBuffer
	Light = new PerFrameConstants;
	ZeroMemory(Light, sizeof(PerFrameConstants));

	Light->mCameraNearFar = XMVECTOR{ D3DGC_Light->NearPlane, D3DGC_Light->FarPlane, 0.0f, 0.0f };

	Light->mFramebufferDimensionsX = D3DGC_Light->ScreenWidth;
	Light->mFramebufferDimensionsY = D3DGC_Light->ScreenHeight;
	Light->mFramebufferDimensionsZ = 0;     // Unused
	Light->mFramebufferDimensionsW = 0;     // Unused

	Light->mUI.lightingOnly = false;
	Light->mUI.clusteredGridScale = 16;
	Light->mUI.shadowsOn = D3DGC_Light->ShadowsOn;
	Light->mUI.softshadowsOn = D3DGC_Light->SoftShadowsOn;
	Light->mUI.PCF_Amount = D3DGC_Light->PCF_Amount;
	Light->mUI.PCF_Step = 1.0f;
	Light->mUI.ShadowCLAMP = 1.0f;
	Light->mUI.Shadow_Divider = 1024.0f;
	// Create constant buffers
	{
		CD3D11_BUFFER_DESC desc(
			sizeof(PerFrameConstants),
			D3D11_BIND_CONSTANT_BUFFER,
			D3D11_USAGE_DEFAULT
			);

		hr = D3DGC_Light->D11_device->CreateBuffer(&desc, 0, &mPerFrameConstants);
		if (FAILED(hr))
		{
			MessageBox(hwnd, L"Could not initialize Clustering mPerFrameConstants.", L"Error", MB_OK);
			return false;
		}
	}

	// Set up macros
	D3D10_SHADER_MACRO defines[] = {
		{ "MSAA_SAMPLES", "1" }, //msaaSamplesStr.c_str()
		{ 0, 0 }
	};
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb172416%28v=vs.85%29.aspx
	UINT shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;// | D3D10_SHADER_OPTIMIZATION_LEVEL3;

	SetAllLightDefault();

	mLightGridBuffer = new StructuredBuffer<LightGridEntry>(D3DGC_Light->D11_device, mLightGridBufferSize, D3D11_BIND_SHADER_RESOURCE, true);

	return true;
}


void LightClass::Frame()
{
// Обновляем все PointLights
	Light->mUI.shadowsOn = D3DGC_Light->ShadowsOn;
	Light->mUI.softshadowsOn = D3DGC_Light->SoftShadowsOn;
	Light->mUI.PCF_Amount = D3DGC_Light->PCF_Amount;
	Light->mUI.PCF_Step = D3DGC_Light->PCF_Step;
	Light->mUI.ShadowCLAMP = D3DGC_Light->ShadowCLAMP;
	Light->mUI.Shadow_Divider = D3DGC_Light->Shadow_Divider;

	lightBufferSRV = FrameLights(D3DGC_Light->D11_deviceContext, &Light->mUI);

	D3DGC_Light->D11_deviceContext->UpdateSubresource(mPerFrameConstants, NULL, NULL, Light, NULL, NULL);
	D3DGC_Light->D11_deviceContext->PSSetConstantBuffers(1, 1, &mPerFrameConstants);

}


void LightClass::SetAllLightDefault()
{
//	ZeroMemory(Light, sizeof(LightBufferType));
	  Light->AmbientColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	  Light->DiffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	  Light->DiffuseDirection = XMFLOAT3(100.0f, 100.0f, 100.0f);
	  Light->DiffusePower = 2.0f;

	  Light->SpecularColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	  Light->SpecularDirection = Light->DiffuseDirection;//XMFLOAT3(0.0f, 0.0f, 1.0f);
	  Light->SpecularPower = 0.002f;//0.002f;
}


int LightClass::AddLightSource(PointLight &NewLight)
{
	int Number;

	// Указатель на новый или старый свет который будет инициализирован
	PointLight* CreateLight = NULL;

	if ( ! FreeLightIndex.empty() )
	{
		Number = ( int ) FreeLightIndex.back();
		FreeLightIndex.pop_back();

		CreateLight = mPointLightParameters[Number];
		memcpy( CreateLight, &NewLight, PointLightSize );
	}
	else
	{
		Number = (int)mPointLightParameters.size();

		// Общее количестмо светов на сцене не должно привышать MAX_LIGHTS - 4096
		if ( Number > MAX_LIGHTS )
			return -1;

		CreateLight = new PointLight;
		ZeroMemory( CreateLight, PointLightSize );
		memcpy( CreateLight, &NewLight, PointLightSize );
		// Создаём свет
		mPointLightParameters.emplace_back( CreateLight );

		// Увеличиваем количество светов на сцене на 1
		mActiveLights = Number + 1;

		// Пересоздаём массив светов нужного размера
		ResetLightsBuffer();
	}
	// Сохраняем индекс света для отрисовки.  Если свет не рисуется, то Dummy = -1
	CreateLight->Dummy = Number;

	// Если добавляется свет с тенью, то удаляем массив Shadow Maps 
	// чтобы создать новый + 1 для нового Spot Light света с тенью
	if (CreateLight->HaveShadow && mActiveLightsWithShadows < MAX_LIGHTS_WITH_SHADOW)
	{
		DeleteShadowMapsArray(ShadowMap3D, SRV_ShadowMap3D);
		// Создаём новый массив Shadow Maps + 1 для нового света
		if (!CreatingShadowMapsArray(
			(int)LightViewPort.Width,
			(int)LightViewPort.Height,
			D3DGC_Light->MSAAQualityCount,
			D3DGC_Light->MSAAQualityChoosen,
			++mActiveLightsWithShadows // Количество светов с тенью + 1
			))
			return -2; // Не могу создать массив Shadow Maps для теней
		// добавляем в список индексов SpotLight которым нужен ShadowMap
		SpotLightsWithShadowsIndexes.emplace_back(Number);
	}

	return Number;
}


void LightClass::ResetLightsBuffer ()
{
	// Удаляем буфер светов, чтобы добавить ещё один свет к существующим.
	RCUBE_DELETE ( mLightBuffer );
	// Создаём буфер светов передаваемый в шейдер
	mLightBuffer = new StructuredBuffer<PointLight> ( D3DGC_Light->D11_device, mActiveLights, D3D11_BIND_SHADER_RESOURCE, true );
}



int LightClass::GetActiveLghtNumber()
{
return mActiveLights; ;//(int)FreeLightIndex.size();//mActiveLights;
}


void LightClass::FreeLightSource ( int LightNumber )
{
	if ( LightNumber < ( int ) mPointLightParameters.size() )
	{
		FreeLightIndex.emplace_back( LightNumber );		// Помещаем индекс освободившегося света в массив индексов созданных, но не используемых светов
		mPointLightParameters[LightNumber]->Dummy = -1;	// Убираем свет из отрисовки
		// Проверяем, была ли у света тень для удаления тени из отрисовки ShadowMap
		if ( mPointLightParameters[LightNumber]->ShadowMapSliceNumber != -1 )
		{
			int Amount = ( int ) SpotLightsWithShadowsIndexes.size ();
			for ( int i = 0; i < Amount; i++ )
			{
			// Удаляем индекс света из списка светов для которых генеится ShadowMap
				if ( SpotLightsWithShadowsIndexes[i] == LightNumber )
				{
					SpotLightsWithShadowsIndexes.erase( SpotLightsWithShadowsIndexes.begin () + i );
					// Уменьшаем на 1 количество светов с тенью
					--mActiveLightsWithShadows;
					break;
				}
			}
		}

	}
}


bool LightClass::DeleteLightSource(int LightNumber)
{
	int i = (int)mPointLightParameters.size();
	if (LightNumber < i)
	{
		delete mPointLightParameters[LightNumber];
		mPointLightParameters.erase(mPointLightParameters.begin() + LightNumber);
		--mActiveLights;

		ResetLightsBuffer ();

		return true;
	}
	else
		return false;
}


void LightClass::DeleteAllLights()
{
	int Number = (int)mPointLightParameters.size();

//#pragma loop(ivdep)
//#pragma loop(hint_parallel(0))
	for (int i = 0; i < Number; ++i)
	{
		delete mPointLightParameters[i];
	}

	RCUBE_DELETE(mLightBuffer);
	// Очищаем буфер светов
	mPointLightParameters.clear();
	// Очищаем буфер индексов светов с тенями
	SpotLightsWithShadowsIndexes.clear();
	// Очищаем буфер свободных индексов для света
	FreeLightIndex.clear();

	DeleteShadowMapsArray(ShadowMap3D, SRV_ShadowMap3D);
}


bool LightClass::ChangeLightDirection(int LightNumber, XMFLOAT3& direction)
{
	if ( LightNumber > (int)mPointLightParameters.size())
		return false;
	
	mPointLightParameters[LightNumber]->direction = direction;

	return true;
}


bool LightClass::ChangeLightDirection(int LightNumber, XMVECTOR& direction)
{
	if (LightNumber > (int)mPointLightParameters.size())
		return false;

	Temp.Vec = direction;
	mPointLightParameters[LightNumber]->direction = Temp.Fl3;

	return true;
}


bool LightClass::ChangeLightAttnuationBegin( int LightNumber, float& attBegin )
{
	if ( LightNumber > ( int ) mPointLightParameters.size() )
		return false;

	mPointLightParameters[LightNumber]->attenuationBegin = attBegin;

	return true;
}


bool LightClass::ChangeLightAttnuationEnd( int LightNumber, float& attEnd )
{
	if ( LightNumber > ( int ) mPointLightParameters.size() )
		return false;

	mPointLightParameters[LightNumber]->attenuationEnd = attEnd;

	return true;
}


bool LightClass::ChangeLightColor( int LightNumber, XMFLOAT3& color )
{
	if ( LightNumber > ( int ) mPointLightParameters.size() )
		return false;

	mPointLightParameters[LightNumber]->color = color;

	return true;
}


bool LightClass::ChangeLightDirection(int LightNumber, XMVECTOR&Axis, float Angel)
{
	if (LightNumber > (int)mPointLightParameters.size())
		return false;

	XMMATRIX Rot = XMMatrixRotationAxis(Axis, Angel);
	Temp.Fl3 = mPointLightParameters[LightNumber]->direction;
	Temp2.Vec = XMVector3TransformCoord(Temp.Vec, Rot);
	mPointLightParameters[LightNumber]->direction = Temp2.Fl3;

	return true;
}


bool LightClass::GetLightDirection(int LightNumber, XMFLOAT3& direction)
{
	if (LightNumber > (int)mPointLightParameters.size())
		return false;
	
	direction = mPointLightParameters[LightNumber]->direction;

	return true;
}


bool LightClass::GetLightDirection(int LightNumber, XMVECTOR& direction)
{
	if (LightNumber > (int)mPointLightParameters.size())
		return false;

	Temp.Fl3 = mPointLightParameters[LightNumber]->direction;
	direction = Temp.Vec;

	return true;
}


bool LightClass::GetLightPos(int LightNumber, XMFLOAT3& pos)
{
	if (LightNumber > (int)mPointLightParameters.size())
		return false;
	
	pos = mPointLightParameters[LightNumber]->position;

	return true;
}


bool LightClass::GetLightPos(int LightNumber, XMVECTOR& pos)
{
	if (LightNumber > (int)mPointLightParameters.size())
		return false;
	
	Temp.Fl3 = mPointLightParameters[LightNumber]->position;
	pos = Temp.Vec;

	return true;
}


void LightClass::LightsOnly()
{
	Light->mUI.lightingOnly = true;
}


void LightClass::LightsOnlyOFF()
{
	Light->mUI.lightingOnly = false;
}


void LightClass::SetClustersAmount(int amount)
{
	if ( amount == 4 || amount == 8 || amount == 16 || amount == 24 || amount == 32 )
	Light->mUI.clusteredGridScale = amount;
}


bool LightClass::ChangeLightPos(int LightNumber, XMFLOAT3& pos)
{
	if (LightNumber > (int)mPointLightParameters.size())
		return false;
	mPointLightParameters[LightNumber]->position = pos;
		return true;
}


bool LightClass::ChangeLightPos(int LightNumber, XMVECTOR& pos)
{
	if (LightNumber > (int)mPointLightParameters.size())
		return false;
	Temp.Vec = pos;
	mPointLightParameters[LightNumber]->position = Temp.Fl3;
	return true;

}


void LightClass::RenderIndextedClustered(ID3D11ShaderResourceView *ModelTexture,
	int indexDrawAmount, int InstancCout) {

	D3DGC_Light->D11_deviceContext->PSSetShaderResources(0, 1, &ModelTexture);
	D3DGC_Light->D11_deviceContext->PSSetShaderResources(1, 1, &SRV_ShadowMap3D);
	D3DGC_Light->D11_deviceContext->PSSetShaderResources(5, 1, &lightBufferSRV);
	D3DGC_Light->D11_deviceContext->PSSetShaderResources(6, 1, &LightGridBufferSRV);
	// instance модель или нет ?  >1 = true
	if (InstancCout > 1)
		D3DGC_Light->D11_deviceContext->DrawIndexedInstanced(indexDrawAmount, InstancCout, 0, 0, 0);
	else
		D3DGC_Light->D11_deviceContext->DrawIndexed(indexDrawAmount, 0, 0);


}


void LightClass::RenderClustered(ID3D11ShaderResourceView *ModelTexture,
	int vertexDrawAmount, int InstancCout)
{

	D3DGC_Light->D11_deviceContext->PSSetShaderResources(0, 1, &ModelTexture);
	D3DGC_Light->D11_deviceContext->PSSetShaderResources(1, 1, &SRV_ShadowMap3D);
	D3DGC_Light->D11_deviceContext->PSSetShaderResources(5, 1, &lightBufferSRV);
	D3DGC_Light->D11_deviceContext->PSSetShaderResources(6, 1, &LightGridBufferSRV);
	// instance модель или нет ?  >1 = true
	if (InstancCout > 1)
		D3DGC_Light->D11_deviceContext->DrawInstanced(vertexDrawAmount, InstancCout, 0, 0);
	else
		D3DGC_Light->D11_deviceContext->Draw(vertexDrawAmount, 0);

}


void LightClass::DeleteShadowMapsArray(
	ID3D11Texture2D* Texture,
	ID3D11ShaderResourceView* SRV
	)
{
	RCUBE_RELEASE(SRV);
	RCUBE_RELEASE(Texture);
}


bool LightClass::CreatingShadowMapsArray(

	int Width,
	int Height,
	UINT MsaaCount,
	UINT MsaaQuality,
	int ArraySize
	)
{
	HRESULT res;

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = Width;
	depthStencilDesc.Height = Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = ArraySize;
	depthStencilDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthStencilDesc.SampleDesc.Count = MsaaCount;
	depthStencilDesc.SampleDesc.Quality = MsaaQuality;

	res = D3DGC_Light->D11_device->CreateTexture2D(&depthStencilDesc, NULL, &ShadowMap3D);
	if (FAILED(res))
	{
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	// Если используется MSAA, то нужно иначе создать текстуру
	if (D3DGC_Light->MSAAQualityCount > 1)
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
		srvDesc.Texture2DMSArray.ArraySize = depthStencilDesc.ArraySize;
		srvDesc.Texture2DMSArray.FirstArraySlice = 0;
	}
	else
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray.MipLevels = depthStencilDesc.MipLevels;
		srvDesc.Texture2DArray.MostDetailedMip = 0;
		srvDesc.Texture2DArray.ArraySize = depthStencilDesc.ArraySize;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
	}


	res = D3DGC_Light->D11_device->CreateShaderResourceView(ShadowMap3D, &srvDesc, &SRV_ShadowMap3D);
	if (FAILED(res))
	{
		return false;
	}

	return true;
}


void LightClass::ShadowsON ( bool Value )
{
	Light->mUI.shadowsOn = Value;
}


void LightClass::SoftShadowsON ( bool Value )
{
	Light->mUI.softshadowsOn = Value;
}


void LightClass::LightRender(int LightNumber, bool Yes)
{
	if (LightNumber > (int)mPointLightParameters.size())
		return;
	else if (Yes)
		mPointLightParameters[LightNumber]->Dummy = LightNumber;
	else
		mPointLightParameters[LightNumber]->Dummy = -1;
}


/*
D3DGC_Light->D11_deviceContext->CopyResource(D3DGC_Light->ScreenShootTexture, D3DGC_Light->backBufferPtr);
union _block
{
byte  *data;
unsigned int *udata;
} block;

D3D11_MAPPED_SUBRESOURCE  mapResource;
hr = D3DGC_Light->D11_deviceContext->Map(D3DGC_Light->ScreenShootTexture, 0, D3D11_MAP_READ, NULL, &mapResource);

unsigned int amount = mapResource.DepthPitch; // RowPitch * Height;
block.data = new byte[amount];
memcpy(block.data, mapResource.pData, amount);
D3DGC_Light->D11_deviceContext->Unmap(D3DGC_Light->ScreenShootTexture,0);
Image img;
D3D11_TEXTURE2D_DESC* TextureDesc = new D3D11_TEXTURE2D_DESC;
D3DGC_Light->ScreenShootTexture->GetDesc(TextureDesc);
img.height = TextureDesc->Height;
img.width = TextureDesc->Width;
img.format = TextureDesc->Format;
img.rowPitch = mapResource.RowPitch;
img.slicePitch = mapResource.DepthPitch;
img.pixels = block.data;

hr = SaveToWICFile(img, WIC_FLAGS_NONE, GetWICCodec(WIC_CODEC_PNG), L"1___BACKBUFFER.png");
if (FAILED(hr))
{
assert(L"Could not save 1___BACKBUFFER.png file.");
}

delete[] block.data;
delete TextureDesc;
*/
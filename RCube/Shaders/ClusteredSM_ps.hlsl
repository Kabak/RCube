// Copyright 2014 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.

#ifndef CLUSTERED_HLSL
#define CLUSTERED_HLSL

#pragma pack_matrix( row_major )

//--------------------------------------------------------------------------------------
// Lighting phase utilities
//--------------------------------------------------------------------------------------
struct Light
{
	float3 positionView;
	float attenuationBegin;
	float3 color;
	float attenuationEnd;
	float3 direction;
	float angel;
	matrix qtwvp;
//	float4 RotQuat;
	uint haveShadow;
	float ShadowMapSliceNumber;
	int LightID;
	int Dummy;
};


StructuredBuffer<Light> gLight : register(t5);
StructuredBuffer<uint4> gLightGrid : register(t6);

//--------------------------------------------------------------------------------------
// Geometry phase
//--------------------------------------------------------------------------------------
Texture2D gDiffuseTexture : register(t0);
Texture2DArray ShadowMap3D : register(t1);

SamplerState Model_Texture_Sampler : register(s0);
SamplerState ShadowMap_Sampler  : register(s1);
SamplerComparisonState SM_PCF_Sampler : register(s2);

struct GeometryVSOut
{
	float4 position     : SV_Position;
	float4 positionView : positionView;      // View space position
	float3 normal       : normal;
	float2 texCoord     : texCoord;
	float3 viewDirection : TEXCOORD1;
//	float4 lightViewPosition : TEXCOORD2;
//	matrix TransRotObjMatrix : TEXCOORD3;
};


struct UIConstants
{
	uint lightingOnly;
	uint shadowsOn;
	uint clusteredGridScale;
	uint softShadowsOn;
   float PCF_Amount;
   float PCF_Step;
   float ShadowCLAMP;
   float Shadow_Divider;
};

cbuffer PerFrameConstants : register(b1)
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 diffuseDirection;
	float DifusePower;
	float4 specularColor;
	float3 specularDirection;
	float SpecularPower;

	float4 mCameraNearFar;
	uint4 mFramebufferDimensions;

	UIConstants mUI;
};

// Data that we can read or derive from the surface shader outputs
struct SurfaceData
{
	float3 positionView;         // View space position
	float specularAmount;        // Treated as a multiplier on albedo
	float3 normal;               // View space normal
//	uint shadowsOn;
//	float specularPower;
	float4 albedo;
	float3 viewDirection;		//  Я добавил
//	float shadowMapBias;
//	float3 WorldPos;
//	float3 positionViewDX;
//	float3 positionViewDY;

	float4  lightViewPosition;
//	uint softShadowsOn;
//	matrix TransRotObjMatrix;
	float3 ObjVertexPos;
};


void fill_array4(out int array[4], int4 src)
{
	array[0] = src.x;
	array[1] = src.y;
	array[2] = src.z;
	array[3] = src.w;
}

/*
float2 DepthGradient( float2 uv, float z, float3 DDX, float3 DDY )
{
	float2 dz_duv = 0;

	float3 duvdist_dx = DDX;//ddx( float3( uv, z ) );
	float3 duvdist_dy = DDY;//ddy( float3( uv, z ) );

	dz_duv.x = duvdist_dy.y * duvdist_dx.z;
	dz_duv.x -= duvdist_dx.y * duvdist_dy.z;

	dz_duv.y = duvdist_dx.x * duvdist_dy.z;
	dz_duv.y -= duvdist_dy.x * duvdist_dx.z;

	float det = ( duvdist_dx.x * duvdist_dy.y ) - ( duvdist_dx.y * duvdist_dy.x );
	dz_duv /= det;

	return dz_duv;
}


float BiasedZ( float z0, float2 dz_duv, float2 offset )
{
	return z0 + dot( dz_duv, offset );
}



// Функция умножения позиции объекта на кватернион  без преобразования в матрицу
float3 qtransform(float4 q, float3 v)
{
	return v + 2.0 * cross(cross(v, q.xyz) + q.w * v, q.xyz);
}

// Кватернион в Матрицу 
matrix QuatToMatrix(float4 Quat) {

	float x22 = Quat.x * Quat.x * 2;
	float y22 = Quat.y * Quat.y * 2;
	float z22 = Quat.z * Quat.z * 2;

	float xy2 = Quat.x * Quat.y * 2;
	float wz2 = Quat.w * Quat.z * 2;

	float xz2 = Quat.x * Quat.z * 2;
	float wy2 = Quat.w * Quat.y * 2;

	float yz2 = Quat.y * Quat.z * 2;
	float wx2 = Quat.w * Quat.x * 2;


	matrix Identity = { 1.0 - y22 - z22 , xy2 + wz2 , xz2 - wy2 , 0.0 ,
		xy2 - wz2, 1.0 - x22 - z22 , yz2 + wx2 , 0.0 ,
		xz2 + wy2 , yz2 - wx2, 1.0 - x22 - y22 , 0.0 ,
		0.0, 0.0, 0.0, 1.0 };

	return Identity;
}
*/
//--------------------------------------------------------------------------------------
// Utility
//--------------------------------------------------------------------------------------
float3 texOffset(float3 Offset)
{
	float3 result = 0.0f;

	result.xy = Offset.xy * Offset.z;

	return result;
}

float linstep(float min, float max, float v)
{
	return saturate((v - min) / (max - min));
}


float3 ComputeFaceNormal(float3 position)
{
	return cross(ddx_coarse(position), ddy_coarse(position));
}

SurfaceData ComputeSurfaceDataFromGeometry(GeometryVSOut input)
{
	SurfaceData surface;
	surface.positionView = input.positionView.xyz;
	surface.viewDirection = input.viewDirection;
// Позиция объекта для расчёта затухания тени	
	surface.ObjVertexPos = input.position.xyz;

//	surface.TransRotObjMatrix = input.TransRotObjMatrix;
	// These arguably aren't really useful in this path since they are really only used to
	// derive shading frequencies and composite derivatives but might as well compute them
	// in case they get used for anything in the future.
	// (Like the rest of these values, they will get removed by dead code elimination if
	// they are unused.)
//	surface.positionViewDX = ddx_coarse(surface.positionView);
//	surface.positionViewDY = ddy_coarse(surface.positionView);

// ++++++++++++++++++++   Я ДОБАВИЛ   ++++++++++++++++++++++++++++++++
//	surface.lightToPixelVec = input.position.xyz - input.WorldPos.xyz;
//	surface.SurfNormal = ComputeFaceNormal(input.SurfNormal); //ComputeFaceNormal(input.positionView);
// ++++++++++++++++++++   Я ДОБАВИЛ   ++++++++++++++++++++++++++++++++

// Переключатель нормалей 
// Optionally use face normal instead of shading normal
//	surface.normal = normalize(ComputeFaceNormal(input.positionView.xyz));
	surface.normal = input.normal;
//	float3 faceNormal = ComputeFaceNormal(input.positionView);
//	surface.normal = normalize(mUI.faceNormals ? faceNormal : input.normal);
// -----------------------------------------------------------
// Переключаемся: рисуем текстуру или закрышиваем белым цветом
	surface.albedo = gDiffuseTexture.Sample( Model_Texture_Sampler, input.texCoord);
	surface.albedo.rgb = mUI.lightingOnly ? float3(1.0f, 1.0f, 1.0f) : surface.albedo.rgb;
// -----------------------------------------------------------
	// Map NULL diffuse textures to white
	uint2 textureDim;
	gDiffuseTexture.GetDimensions(textureDim.x, textureDim.y);
	surface.albedo = (textureDim.x == 0U ? float4(1.0f, 1.0f, 1.0f, 1.0f) : surface.albedo);


	// We don't really have art asset-related values for these, so set them to something
	// reasonable for now... the important thing is that they are stored in the G-buffer for
	// representative performance measurement.
	surface.specularAmount = 0.9f;//6.7f; //0.17f;//0.9f;//
//	surface.specularPower = mUI.specularPower;
// ТЕНИ
//	surface.shadowMapBias = 0.0f;//mUI.shadowMapBias;
//	surface.shadowsOn = mUI.shadowsOn;
//	surface.softShadowsOn = mUI.softShadowsOn;
	surface.lightViewPosition = input.positionView;
	return surface;
}


//--------------------------------------------------------------------------------------
// Lighting phase utilities
//--------------------------------------------------------------------------------------

// As below, we separate this for diffuse/specular parts for convenience in deferred lighting
void AccumulatePhongBRDF(
	float3 normal,
	float3 lightDir,
	float distance,
	float3 viewDir,
	inout float3 lightContrib,
	float specularPower,
	inout float specular,
	inout float NdotL,
	float3 direction,
	float angel)
{

	NdotL = saturate(dot(normal, lightDir));
	lightContrib /= 0.4f + 0.02f * distance;

	if (angel > 0.0f)
	{
		// direction - направление источника света
		float LightDot = dot(-lightDir, normalize(direction));

		float spread = 360.0f / angel;
		lightContrib *= 20 * pow(max(LightDot, 0.0f), 0.4f + 0.02f * distance * spread);
	}

	if (NdotL > 0.0f)
	{
		// Для зеркальности особенно 2-е след. строки
		float3 r = normalize(2 * NdotL * normal - lightDir );
		//float3 r = reflect(NdotL, normal); //  Почему-то неправильно расположено пятно Specular
		//		float3 r = reflect(lightDir, normal);
		// Для зеркальности особенно 2-е след. строки
		float RdotV = saturate(dot(r, viewDir));
		specular = pow(RdotV, 1.0f / specularPower);
//		specular = 4 * pow(RdotV, 1.0f / specularPower);
//		specular = 4 * pow(RdotV, specularPower);
	}

}
// Цветопередача
// http://www.gamedev.ru/code/forum/?id=230008
// Uses an in-out for accumulation to avoid returning and accumulating 0
void AccumulateBRDF(SurfaceData surface, Light light, inout float3 lit)
{
	float attenuation = 0.0f;
	float3 Contribute = 0.0f;
	float3 litDiffuse = 0.0f;
	float3 litSpecular = 0.0f;
	float specular = 0.0f;
	float NdotL = 0.0f;

	float3 directionToLight = light.positionView - surface.positionView;
	float distanceToLight = length(directionToLight);

	if (distanceToLight < light.attenuationEnd)
	{
		attenuation = linstep(light.attenuationEnd, light.attenuationBegin, distanceToLight);
		directionToLight *= rcp(distanceToLight);       // A full normalize/RSQRT might be as fast here anyways...

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//				Проблемное место по litDiffuse. проступает тень от "Солнца" рядом с солнцем, если включена тень
//				при полной темноте   

		float3 AtLight = (attenuation * light.color);

		Contribute = saturate(AtLight / (1.0f + (0.005f * distanceToLight)));

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		AccumulatePhongBRDF(
			surface.normal,
			directionToLight,
			distanceToLight,

			surface.viewDirection,
			Contribute,
			SpecularPower,
			specular,
			NdotL,

			light.direction,
			light.angel
			);


		// ++++++++++++     Обсчёт теней     ++++++++++++++
		if ( mUI.shadowsOn && light.haveShadow )
		{
			// подставляем матрицу от конкретного источника света 
			// для корректного наложения SpotLIght Shdow Map
//			if (light.LightID == light.ShadowMapSliceNumber)
			{
				// QUATERNION
				//			matrix M = mul(WorldTransRotObjMatrix, QuatToMatrix(light.qtwvp));
				////			float4 LightPos = float4(qtransform(light.qtwvp, surface.lightViewPosition.xyz) , 1.0f);
				//			surface.lightViewPosition = mul(surface.lightViewPosition, M);
				// MATRIX
	//			matrix M = mul(surface.TransRotObjMatrix, light.qtwvp);
				surface.lightViewPosition = mul(surface.lightViewPosition, light.qtwvp);

//				surface.lightViewPosition = float4( qtransform( light.RotQuat, surface.lightViewPosition.xyz ), 1.0f );
//				matrix M = QuatToMatrix( light.RotQuat );
//				surface.lightViewPosition = mul( surface.lightViewPosition, M );
			}


			// Set the bias value for fixing the floating point precision issues.
			float4 projectTexCoord = 0;
			float shadowMapFactor = 0.0f;
			//re-homogenize position after interpolation
			surface.lightViewPosition.xyz /= surface.lightViewPosition.w;

//			DepthGradient( surface.lightViewPosition.xy, surface.lightViewPosition.z );
//			projectTexCoord.x = surface.lightViewPosition.x / 2.0f + 0.5f;
//			projectTexCoord.y = surface.lightViewPosition.y / -2.0f + 0.5f;
			float2 a = {2.0f,-2.0f};
			projectTexCoord.xy = surface.lightViewPosition.xy / a.xy + 0.5f;

			projectTexCoord.z = light.ShadowMapSliceNumber;

//			float2 SatXY = saturate ( projectTexCoord.xy );
			// Check if the projected coordinates are in the view of the light, if not then the pixel gets just an ambient value. 
//			if ( SatXY.x == projectTexCoord.x && SatXY.y == projectTexCoord.y )
			{
				// Смотрим текстуру теней
				float shadowMapDepth = ShadowMap3D.SampleGrad( ShadowMap_Sampler, projectTexCoord.xyz, 0.0f, 0.0f).r;

				if (surface.lightViewPosition.z  > shadowMapDepth)
				{
					// PCF фильтрация, если включены мягкие тени
					if ( mUI.softShadowsOn )
					{
						// http://http.developer.nvidia.com/GPUGems/gpugems_ch11.html
						// https://takinginitiative.wordpress.com/2011/05/25/directx10-tutorial-10-shadow-mapping-part-2/
						// https://habrahabr.ru/post/259679/
						//PCF sampling for shadow map

						float sum = 0.0f;
						float3 Offset;	// Для скорости  всё загнал во float3
						Offset.z = 1.0f / mUI.Shadow_Divider;

						//perform PCF filtering on a 4 x 4 texel neighborhood
						float val = mUI.PCF_Amount;
						float Step = mUI.PCF_Step;
						float Devider = ( val * 2 ) * ( val / Step );
						for ( Offset.y = -val; Offset.y <= val; Offset.y += Step )
						{
							for ( Offset.x = -val; Offset.x <= val; Offset.x += Step )
							{
								sum += ShadowMap3D.SampleCmpLevelZero( SM_PCF_Sampler, projectTexCoord.xyz + texOffset(Offset), surface.lightViewPosition.z ).r;
							}
						}

						shadowMapFactor += sum / Devider;

					}
// + Эксперименты
					// Делаем чтобы тень затухала по мере увеличения расстояния от источника света
					// чем юольше множитель light.attenuationEnd * 0.9f - близко к идеалу, 
					// тем контрастнее тень на границе засветки источником света
					// float attenuation2 = linstep(light.attenuationEnd, light.attenuationBegin, distanceToLight );
					// attenuation2 запихиваем сюда чтобы тени уменьшались по мере удаления от источника света
					// attenuation2 * 0.55f - чтобы тень была не очень контрастная около источника света
					// чем больше множитель, тем темнее тень
// - Эксперименты

					float3 BorderContribute = saturate(min(Contribute, shadowMapFactor * 1.64f));
					litDiffuse += saturate(Contribute * BorderContribute * NdotL);
					lit += surface.albedo.rgb * litDiffuse;
					return;
				}
				// ++++++++++++     ДЛЯ СВЕТОВ С ТЕНЬЮ       ++++++++++++++++
			}
			// ++++++++++++     ДЛЯ СВЕТОВ НЕ ПОПАВШИХ ВО FRUSTUM СВЕТА       ++++++++++++++++
		//			return;
		}
		// ++++++++++++     ДЛЯ СВЕТОВ БЕЗ ТЕНЕЙ | ВСЕХ СВЕТОВ    ++++++++++++++++
		// 1
		//	litDiffuse +=  Contribute * NdotL;
		//	litSpecular += Contribute * specular;
		//	lit += surface.albedo.rgb * litDiffuse;
		//	lit += surface.specularAmount * litSpecular;
		// -------------------------------------------------------------
		// 2
		litDiffuse += saturate(Contribute * NdotL);
		litSpecular += saturate(Contribute * specular);
		lit += surface.albedo.rgb * litDiffuse;
		lit += surface.specularAmount * litSpecular;
		// -------------------------------------------------------------
	}

}


float4 ClusteredPS(GeometryVSOut input) : SV_Target
{
	uint3 grid = uint3(2, 1, 8) * mUI.clusteredGridScale;

	SurfaceData surface = ComputeSurfaceDataFromGeometry(input);


//	float3 ambientColor = float3(0.55f, 0.55f, 0.55f);

	// Свойства материала
	// Ambient
	float3 Ka = float3(1.0f, 1.0f, 1.0f);
	// Diffuse
	float3 Kd = float3(1.0f, 1.0f, 1.0f);
	// Specular
	float3 Ks = float3(1.0f, 1.0f, 1.0f);
	// Прозрачность
	float d = 0.0f;
	// Блестящая поверхность
	float Ns = 0.0f;
	// Излучающая поверхность
	float illum = 0.0f;
// Подмешиваем Ambient по умолчанию в финальный свет текстур
	float3 lit = ambientColor.rgb * Ka * surface.albedo.rgb;

	float2 screenPosition = input.position.xy / mFramebufferDimensions.xy;
	float zPosition = (surface.positionView.z - mCameraNearFar.x) / (mCameraNearFar.y - mCameraNearFar.x);

	uint3 clusterPosition = uint3(screenPosition*grid.xy, zPosition * grid.z);
	uint cluster_index = (clusterPosition.y * grid.x + clusterPosition.x) * grid.z + clusterPosition.z;

	int lightIndexBlock[4];
	fill_array4(lightIndexBlock, gLightGrid[cluster_index]);

	int list_size = lightIndexBlock[0] & 255;
	int list_index = lightIndexBlock[0] >> 8;
	int light_count = list_size;

	[loop]
	for (int k = 2; k < list_size + 2; ++k)
	{
		int lightIndex = (lightIndexBlock[(k & 7) >> 1] >> ((k & 1) << 4)) & 0xFFFF;
		if ((k & 7) == 7) fill_array4(lightIndexBlock, gLightGrid[list_index++]);

		Light light = gLight[lightIndex];
// Если свет не используется, то и не пытаемся его рисовать
		if ( light.Dummy > -1 )
		AccumulateBRDF(surface, light, lit);
	}
	return float4(saturate(lit), 1.0f);
}

/*
	[flatten] if (mUI.visualizeLightCount)
	{
		lit = (float(light_count) * rcp(255.0f)).xxx;
	}
*/
#endif // CLUSTERED_HLSL

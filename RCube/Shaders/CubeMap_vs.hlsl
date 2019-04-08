#pragma pack_matrix( row_major )

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix viewprojection;
	matrix OrthoMatrix;
	matrix ScaleMatrix;
	float3 cameraPosition;
	float padding;
	float4 ViewTransQuat;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;
TextureCube SkyMap;


struct Input_VS
{
	float4 inPos		: POSITION;
	float4 inTexCoord	: TEXCOORD;
	float4 color		: OMGCOLOR;
};

struct SKYMAP_VS_OUTPUT	
{
	float4 Pos			: SV_POSITION;
	float3 texCoord		: TEXCOORD;
};

SKYMAP_VS_OUTPUT SKYMAP_VS( Input_VS Input )
{
	SKYMAP_VS_OUTPUT output = (SKYMAP_VS_OUTPUT)0;

	output.Pos = mul( Input.inPos, ScaleMatrix);

	output.texCoord = Input.inPos.xyz;

	return output;
}
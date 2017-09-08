cbuffer cbPerObject
{
	float4x4 WVP;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
};


float4 PS(VS_OUTPUT input) : SV_TARGET
{
	return ObjTexture.Sample(ObjSamplerState, input.TexCoord);
}
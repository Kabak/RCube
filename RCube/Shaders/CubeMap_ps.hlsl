Texture2D ObjTexture;
SamplerState FlatObject : register (s3);
TextureCube SkyMap;

struct SKYMAP_VS_OUTPUT	//output structure for skymap vertex shader
{
	float4 Pos : SV_POSITION;
	float3 texCoord : TEXCOORD;
};

float4 SKYMAP_PS(SKYMAP_VS_OUTPUT input) : SV_Target
{
	return SkyMap.Sample( FlatObject, input.texCoord);
}

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};


Texture2D shaderTexture;
SamplerState FlatObject : register (s3);


float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor = shaderTexture.Sample( FlatObject, input.tex);

	return textureColor;
}
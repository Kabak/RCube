struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};


Texture2D shaderTexture;
SamplerState SampleType;


float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor = shaderTexture.Sample(SampleType, input.tex);

// Для анимации огня
//		textureColor.a = 1.0 / textureColor.g * textureColor.r * textureColor.b;
		textureColor.a = length( textureColor.rgb);
	return textureColor;
}
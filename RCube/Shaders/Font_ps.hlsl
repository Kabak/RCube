struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

Texture2D shaderTexture;
SamplerState SampleType;

cbuffer PixelBuffer : register(b13)
{
	float4 pixelColor;
};


float4 FontPixelShader(PixelInputType input) : SV_TARGET
{
	float4 color;


// Sample the texture pixel at this location.
color = shaderTexture.Sample(SampleType, input.tex);

//float mul = color.r;
//float mul1 = mul * color.g;
//float mul2 = mul1 * color.b;

// «¿–≈Ã»À ƒÀﬂ Œ“–»—Œ¬ » ÃÕŒ√Œ÷¬≈“ÕŒ√Œ ‘ŒÕ“¿
/*
// If the color is black on the texture then treat this pixel as transparent.
if (color.r == 1.0f && color.g == 1.0f && color.b == 1.0f)
//if ( mul2 == 1.0f )
{
	color.a = 1.0f;
}

// If the color is other than black on the texture then this is a pixel in the font so draw it using the font pixel color.
else
{
	color.a = 0.0f;
}
color = color * pixelColor;
*/

return color;
}
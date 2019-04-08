struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex		: TEXCOORD0;

	float4 TextControl		: Control;
//	float4 color2			: COLOR;
};

Texture2D shaderTexture;

// https://msdn.microsoft.com/en-us/library/windows/desktop/dn899207(v=vs.85).aspx
SamplerState FlatObject: register (s03);

float4 FontPixelShader(PixelInputType input) : SV_TARGET
{
	float4 color = shaderTexture.Sample ( FlatObject, input.tex ) * input.TextControl;

	return color;
}
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex		: TEXCOORD0;

	float4 FlatObjControl	: Control;
	float4 Color			: COLOR;
};


Texture2D shaderTexture;
SamplerState FlatObject : register (s3);


float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor = 0.0f;

	// ColorPicker 
	if ( input.FlatObjControl.w > 0.0f )
	{
		return input.Color;
	}
	// No ColorPicker flat object or ColorPicker with PANTHON sign
	else
	{
		textureColor = shaderTexture.Sample ( FlatObject, input.tex );

		// Animation
		if ( input.FlatObjControl.w < 0.0f )
		{
			textureColor.a = length ( textureColor.rgb );
		}
	}
		
	return textureColor;
}
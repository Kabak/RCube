////////////////////////////////////////////////////////////////////////////////
// Filename: particle_ps.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
SamplerState FlatObject : register (s3);


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float4 color : COLOR;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ParticlePixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float4 finalColor = 0.0f;

	if ( input.position.w > 0.0f )
	{
		// Sample the pixel color from the texture using the sampler at this texture coordinate location.
		textureColor = shaderTexture.Sample ( FlatObject, input.tex );

		// Combine the texture color and the particle color to get the final color result.
		finalColor = textureColor * input.color;
		//	finalColor.a = length( finalColor.rgb );
	}
    return finalColor;
}
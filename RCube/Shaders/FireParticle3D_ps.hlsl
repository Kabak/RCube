////////////////////////////////////////////////////////////////////////////////
// Filename: particle.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
SamplerState SampleType;


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float4 color : COLOR;
	float  Draw : DATA;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ParticlePixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
float4 finalColor = 0.0f;

	if (input.Draw > 0.0f)
	{
		// Sample the pixel color from the texture using the sampler at this texture coordinate location.
		textureColor = shaderTexture.Sample(SampleType, input.tex);

		// Combine the texture color and the particle color to get the final color result.
		//	textureColor.a = length( textureColor.rgb);
		finalColor = textureColor * input.color;
		//	finalColor.a = length( finalColor.rgb );
	}

    return finalColor;
}
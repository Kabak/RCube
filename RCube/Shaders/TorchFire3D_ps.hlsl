////////////////////////////////////////////////////////////////////////////////
// Filename: TorchFire3D_ps.hlsl
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
	float4 finalColor;


    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample( FlatObject, input.tex);

	// Combine the texture color and the particle color to get the final color result.
//	textureColor.a = length( textureColor.rgb);
    finalColor = textureColor * input.color.a;
//	finalColor.a = 0.0f;//input.color.w;
//	finalColor.a = length( finalColor.rgb );
    return finalColor;
}
////////////////////////////////////////////////////////////////////////////////
// Filename: terrain.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////

//We add a texture shader resource for the terrain texture.

Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
};


//////////////
// TYPEDEFS //
//////////////

//Just like the vertex shader file we add a texture coordinate component to the pixel input type structure.

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 TerrainPixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
float3 lightDir;
float lightIntensity;
float4 color;


// Sample the pixel color from the texture using the sampler at this texture coordinate location.
textureColor = shaderTexture.Sample(SampleType, input.tex);

// Set the default output color to the ambient light value for all pixels.
color = ambientColor;

// Invert the light direction for calculations.
lightDir = -lightDirection;

// Calculate the amount of light on this pixel.
lightIntensity = saturate(dot(input.normal, lightDir));

if (lightIntensity > 0.0f)
{
	// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
	color += (diffuseColor * lightIntensity);
}

// Saturate the final light color.
color = saturate(color);

// Multiply the texture pixel and the final light color to get the result.
color = color * textureColor;

return color;
}
#pragma pack_matrix( row_major )

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix viewprojection;
	matrix orthoMatrix;
	matrix ScaleMatrix;
	float3 cameraPosition;
	float padding;
	float4 ViewTransQuat;
};

struct VertexInputType
{
	float4 position			: POSITION;
	float4 tex				: TEXCOORD0;	// only x,y, used as texture coords | z,w - free to use 
	float4 color			: OMGCOLOR;
};


struct PixelInputType
{
	float4 position			: SV_POSITION;
	float2 tex				: TEXCOORD0;

	float4 FlatObjControl	: Control;
	float4 Color			: COLOR;
};


PixelInputType TextureVertexShader(VertexInputType input)
{
	PixelInputType output;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul( input.position, orthoMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex.xy;

	output.FlatObjControl = input.tex.w;	// Color Picker Pixel Shader control

	output.Color = input.color;

	return output;
}

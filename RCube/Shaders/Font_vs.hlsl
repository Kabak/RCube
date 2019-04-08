#pragma pack_matrix( row_major )

cbuffer PerFrameBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix viewprojection;
	matrix OrthoMatrix;
	matrix ScaleMatrix;
	float3 cameraPosition;
	float padding;
	float4 ViewTransQuat;
};

struct VertexInputType
{
	float4 position			: POSITION;
	float4 tex				: TEXCOORD0;	// only x,y,w used     z - free to use 
	float4 color			: OMGCOLOR;		// Free to use  ( used in KF2OBJ for ColorPicker Color )
};


struct PixelInputType
{
	float4 position			: SV_POSITION;
	float2 tex				: TEXCOORD0;

	float4 TextControl		: Control;
//	float4 color2			: COLOR;
};


PixelInputType FontVertexShader(VertexInputType input)
{
	PixelInputType output;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, OrthoMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex.xy;

	output.TextControl = input.tex.w;	// Apply Glow effect if used

//	output.color2 = input.color;

	return output;
}
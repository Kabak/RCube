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
	float4 position		: POSITION;
	float4 tex			: TEXCOORD0;
};


struct PixelInputType
{
	float4 position		: SV_POSITION;
	float2 tex			: TEXCOORD0;
};


PixelInputType TextureVertexShader( VertexInputType input )
{
	PixelInputType output;
	/*
	matrix Identity = { 1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0 };
	*/
	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = input.position;//mul(input.position, Identity);
									 //	output.position = mul(output.position, Identity);
	output.position = mul( output.position, orthoMatrix );

	// The only difference in the texture vertex shader in comparison to the color vertex shader from the previous tutorial is that instead of taking a copy of the color from the input vertex we take 
	// a copy of the texture coordinates and pass them to the pixel shader.

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex.xy;

	return output;
}

cbuffer PerFrameBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix viewprojection;
	matrix OrthoMatrix;
	float3 cameraPosition;
	float padding;
	float4 ViewTransQuat;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};


struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};


PixelInputType FontVertexShader(VertexInputType input)
{
	PixelInputType output;

	matrix Identity = { 1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0 };

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, Identity);
//	output.position = mul(output.position, Identity);
	output.position = mul(output.position, OrthoMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	return output;
}
////////////////////////////////////////////////////////////////////////////////
// Filename: particle.vs
////////////////////////////////////////////////////////////////////////////////
#pragma pack_matrix( row_major )

/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer : register( b0 )
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


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
	float4 position			: POSITION0;
	float4 tex				: TEXCOORD0;

	float4 instancePosition : POSITION1;
	float4 color			: COLOR;
	float4 NewTexCord		: InsTEXCOORD;
};

struct PixelInputType
{
	float4 position			: SV_POSITION;
	float2 tex				: TEXCOORD0;
	float4 color			: COLOR;
};


// Функция умножения позиции объекта на кватернион  без преобразования в матрицу
float3 qtransform( float4 q, float3 v )
{
	return v + 2.0 * cross( cross( v, q.xyz ) + q.w * v, q.xyz );
}

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ParticleVertexShader( VertexInputType input )
{
	PixelInputType output;

	output.position.w = input.instancePosition.w;

	output.position.xyz = qtransform( ViewTransQuat, input.position.xyz ) + input.instancePosition.xyz;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul( output.position, viewprojection );

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex.xy;

	// Store the particle color for the pixel shader. 
	output.color = input.color;

	return output;
}
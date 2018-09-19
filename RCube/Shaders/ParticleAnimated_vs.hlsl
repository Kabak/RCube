////////////////////////////////////////////////////////////////////////////////
// Filename: particle.vs
////////////////////////////////////////////////////////////////////////////////
#pragma pack_matrix( row_major )

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


struct VertexInputType
{
	float4 position				: POSITION0;
	float4 tex					: TEXCOORD0;

	float4 instancePosition		: POSITION1;
	float4 color				: COLOR;
	float4 NewTexCord			: InsTEXCOORD;
};

struct PixelInputType
{
	float4 position				: SV_POSITION;
	float2 tex					: TEXCOORD0;
	float4 color				: COLOR;
//	float  Draw					: DATA;
};


// Функция умножения позиции объекта на кватернион  без преобразования в матрицу
float3 qtransform( float4 q, float3 v )
{
	return v + 2.0 * cross( cross( v, q.xyz ) + q.w * v, q.xyz );
}


float4 mulQuat( float4 a, float4 b )
{
	float4 res;

	res.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
	res.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
	res.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
	res.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;

	return res;
}


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ParticleVertexShader( VertexInputType input, uint VertexNumber : SV_VertexID )
{
	PixelInputType output;

	// Update the position of the vertices based on the data for this particular instance.
	// https://habrahabr.ru/post/255005/
	// https://habrahabr.ru/post/183908/
	// https://habrahabr.ru/post/183116/
	// http://www.gamedev.ru/code/forum/?id=62471
	// http://www.gamedev.ru/code/articles/?id=4175#bliki
	// http://www.gamedev.ru/code/forum/?id=170578

	output.tex = 0.0f;
	output.position = 0.0f;

	output.position.w = input.instancePosition.w;	// Set Render or not

	if (input.instancePosition.w > 0.0f)
	{
		output.position.xyz = qtransform(ViewTransQuat, input.position.xyz) + input.instancePosition.xyz;

		output.position = mul(output.position, viewprojection);

		switch (VertexNumber)
		{
		case 0:
			output.tex.x = input.NewTexCord.y;
			output.tex.y = input.NewTexCord.x;
			break;

		case 1:
			output.tex.x = input.NewTexCord.w;
			output.tex.y = input.NewTexCord.z;
			break;

		case 2:
			output.tex.x = input.NewTexCord.y;
			output.tex.y = input.NewTexCord.z;
			break;

		case 3:
			output.tex.x = input.NewTexCord.w;
			output.tex.y = input.NewTexCord.x;
			break;

		default:
			break;
		}

	}

	// Store the particle color for the pixel shader. 
	output.color = input.color;
//	output.Draw = input.instancePosition.w; // Draw particle 0.0f - Yes   1.0f - No
	return output;
}
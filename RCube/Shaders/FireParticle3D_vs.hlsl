////////////////////////////////////////////////////////////////////////////////
// Filename: particle.vs
////////////////////////////////////////////////////////////////////////////////


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
	float3 position : POSITION0;
	float2 tex : TEXCOORD0;
	float4 instancePosition : POSITION1;
//	float dummy : POSITION2;
	float4 color : COLOR;
	float4 NewTexCord : InsTEXCOORD;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 color : COLOR;
	float  Draw : DATA;
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

	// Change the position vector to be 4 units for proper matrix calculations.
	//    input.position.w = 1.0f;
	//    input.instancePosition.w = 1.0f;

	// Update the position of the vertices based on the data for this particular instance.


	// https://habrahabr.ru/post/255005/
	// https://habrahabr.ru/post/183908/
	// https://habrahabr.ru/post/183116/
	// http://www.gamedev.ru/code/forum/?id=62471
	// http://www.gamedev.ru/code/articles/?id=4175#bliki
	// http://www.gamedev.ru/code/forum/?id=170578

	output.position = 0.0;
	output.tex = 0.0f;

	if (input.instancePosition.w > 0.0f)
	{
		float4 RotQuat = ViewTransQuat;

		output.position = float4(qtransform(ViewTransQuat, input.position.xyz), 1.0f);


		/*

		//	matrix translation = Matrix.CreateTranslation( p.position );
		//	matrix rotation = Matrix.Identity;
		float3 viewUp = { 0.0f, 1.0f, 0.0f };
		float3 RotForvard = normalize( cameraPosition.xyz - input.position.xyz );
		float3 RotRight = cross( viewUp, RotForvard );
		float3 RotUp = cross( RotForvard, RotRight );
		// All those vectors have a length of approx. 1
		//	float fl = length( RotForvard );
		//	float rl = length( RotRight );
		//	float ul = length( RotUp );
		// AddQuad creates the quad and rotates each point by the given matrix
		// Without the rotation, the quads don't show the mentioned squeezing
		//	creator.AddQuad( rotation * translation );
		output.position = float4 ( input.position.xyz, 1.0f );
		//	float3x3 = {}
		//	float4 TempPos = mul ( output.position , )
		*/
		output.position.xyz += input.instancePosition.xyz;
		output.position = mul(output.position, viewprojection);

		//	output.position = output.position * up * right;

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
//			output.tex = 0.0f;
			break;
		}

	}

	// Store the particle color for the pixel shader. 
	output.color = input.color;
	output.Draw = input.instancePosition.w; // Рисовать ли частицу 0.0f - НЕТ   1.0f - ДА
	return output;
}
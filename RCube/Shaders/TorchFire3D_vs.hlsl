////////////////////////////////////////////////////////////////////////////////
// Filename: particle.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer : register(b0)
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


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float3 position : POSITION0;
    float2 tex : TEXCOORD0;
	float3 instancePosition : POSITION1;
	float4 color : COLOR;
	float4 NewTexCord : InsTEXCOORD;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float4 color : COLOR;
};


// Функция умножения позиции объекта на кватернион  без преобразования в матрицу
float3 qtransform(float4 q, float3 v)
{
	return v + 2.0 * cross(cross(v, q.xyz) + q.w * v, q.xyz);
}


float4 mulQuat ( float4 a, float4 b )
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
PixelInputType ParticleVertexShader(VertexInputType input , uint VertexNumber : SV_VertexID )
{
    PixelInputType output;

	float3 NewSize = input.position.xyz;
	
	NewSize.xy *= input.color.x;

	float angelZ = input.color.z; // 0.159154943f
	float SinAngelDiv2Z = sin( angelZ / 2 );
	float CosAngelDiv2Z = cos( angelZ / 2 );

	float4 RotQuatZ = { 0.0f, 0.0f, SinAngelDiv2Z, CosAngelDiv2Z };


	float4 RotQuat = mulQuat(RotQuatZ, ViewTransQuat);

	output.position = float4( qtransform( RotQuat, NewSize ), 1.0f );

	output.position.xyz += input.instancePosition.xyz;
	

	output.position = mul( output.position, viewprojection );
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb509669(v=vs.85).aspx
	// [forcecase]
	[forcecase] switch ( VertexNumber )
	{
	case 0 :
		output.tex.x = input.NewTexCord.y;
		output.tex.y = input.NewTexCord.x;
		break;

	case 1 :
		output.tex.x = input.NewTexCord.w;
		output.tex.y = input.NewTexCord.z;
		break;

	case 2 :
		output.tex.x = input.NewTexCord.y;
		output.tex.y = input.NewTexCord.z;
		break;

	case 3 :
		output.tex.x = input.NewTexCord.w;
		output.tex.y = input.NewTexCord.x;
		break;

	default :
		output.tex = 0.0f;
		break;
	}

    output.color = input.color;

    return output;
}
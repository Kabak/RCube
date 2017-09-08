cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix OrthoMatrix;
	float3 cameraPosition;
	float padding;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;


struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;

};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
};

VS_OUTPUT VS(VertexInputType input)
{
	VS_OUTPUT output;

	float x22 = 0.0f;
	float y22 = 0.0f;
	float z22 = 0.0f;
	float xy2 = 0.0f;
	float wz2 = 0.0f;
	float xz2 = 0.0f;
	float wy2 = 0.0f;
	float yz2 = 0.0f;
	float wx2 = 0.0f;


	matrix TransRotObjMatrix = { 1.0 - y22 - z22 , xy2 + wz2 , xz2 - wy2 , 0.0 ,
		xy2 - wz2, 1.0 - x22 - z22 , yz2 + wx2 , 0.0 ,
		xz2 + wy2 , yz2 - wx2, 1.0 - x22 - y22 , 0.0 ,
		0.0f, 0.0f, 0.0f, 1.0 };


	matrix WVP = mul(worldMatrix, TransRotObjMatrix);
	WVP = mul(WVP, viewMatrix);
	WVP = mul(WVP, projectionMatrix);

	output.Pos = mul(input.position, WVP);
	output.TexCoord = input.tex;
	output.Normal = input.normal;
	output.Tangent = input.tangent;

	return output;
}
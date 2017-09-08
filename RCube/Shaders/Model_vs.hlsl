cbuffer MatrixBuffer
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

Texture2D ObjTexture;
SamplerState ObjSamplerState;


struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float4 instancePosition : ObjPOSITION;
	float4 instanceRotation : ObjROTATION;

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

	float x22 = input.instanceRotation.x * input.instanceRotation.x * 2;
	float y22 = input.instanceRotation.y * input.instanceRotation.y * 2;
	float z22 = input.instanceRotation.z * input.instanceRotation.z * 2;
	float xy2 = input.instanceRotation.x * input.instanceRotation.y * 2;
	float wz2 = input.instanceRotation.w * input.instanceRotation.z * 2;
	float xz2 = input.instanceRotation.x * input.instanceRotation.z * 2;
	float wy2 = input.instanceRotation.w * input.instanceRotation.y * 2;
	float yz2 = input.instanceRotation.y * input.instanceRotation.z * 2;
	float wx2 = input.instanceRotation.w * input.instanceRotation.x * 2;


	matrix TransRotObjMatrix = { 1.0 - y22 - z22 , xy2 + wz2 , xz2 - wy2 , 0.0 ,
		xy2 - wz2, 1.0 - x22 - z22 , yz2 + wx2 , 0.0 ,
		xz2 + wy2 , yz2 - wx2, 1.0 - x22 - y22 , 0.0 ,
		input.instancePosition.x, input.instancePosition.y, input.instancePosition.z, 1.0 };


	matrix WVP = mul(worldMatrix , TransRotObjMatrix);
	WVP = mul(WVP, viewMatrix);
	WVP = mul(WVP , projectionMatrix);

	output.Pos = mul(input.position, WVP);
	output.TexCoord = input.tex;
	output.Normal = input.normal;
	output.Tangent = input.tangent;

	return output;
}
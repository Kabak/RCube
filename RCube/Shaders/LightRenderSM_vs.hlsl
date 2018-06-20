#pragma pack_matrix( row_major )
/*
cbuffer PerFrameConstants : register(b0)
{
	matrix mCameraWorldViewProj;
};
*/
cbuffer MatrixBuffer : register(b0)
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

cbuffer ShadowBuffer : register(b1)
{

	matrix viewprojectionSM;

};

struct GeometryVSIn
{
	float4 position			: POSITION;
	float2 texCoord			: TEXCOORD;
	float3 normal			: NORMAL;
	float3 tangent			: TANGENT;

	float4 instancePosition : ObjPOSITION;
	float4 instanceRotation : ObjROTATION;
};

struct GeometryVSOut
{
	float4 position			: SV_Position;
//	float4 depthPosition	: TEXTURE0;
};

// Функция умножения позиции объекта на кватернион  без преобразования в матрицу
float3 qtransform(float4 q, float3 v)
{
	return v + 2.0 * cross(cross(v, q.xyz) + q.w * v, q.xyz);
}

GeometryVSOut LightRender_VS(GeometryVSIn IN)
{
	GeometryVSOut OUT;

	float4 Temp = 1.0f;

	Temp.xyz = qtransform(IN.instanceRotation, IN.position.xyz) + IN.instancePosition.xyz;

	OUT.position = mul( Temp, viewprojectionSM);
//	OUT.depthPosition = normalize(OUT.position);
	return OUT;
}
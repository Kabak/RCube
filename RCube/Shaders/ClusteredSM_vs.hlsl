// Copyright 2010 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.

#pragma pack_matrix( row_major )

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

struct GeometryVSIn
{
	float4 position				: POSITION;
	float2 texCoord				: TEXCOORD;
	float3 normal				: NORMAL;
	float3 tangent				: TANGENT;

	float4 instancePosition		: ObjPOSITION;
	float4 instanceRotation		: ObjROTATION;
};

struct GeometryVSOut
{
	float4 position				: SV_Position;
	float4 positionView			: positionView;      // View space position
	float3 normal				: normal;
	float2 texCoord				: texCoord;
	float3 viewDirection		: TEXCOORD1;
	// +++++++++++++++++++++++++    Shadow Map    ++++++++++++++++++++++++++++++
//	float4 lightViewPosition : TEXCOORD2;
//	matrix TransRotObjMatrix : TEXCOORD3;
	// -------------------------    Shadow Map    ------------------------------
};
/*
// Кватернион в Матрицу 
matrix QuatToMatrix(float4 Quat, float3 InputPosition ) {

	float x22 = Quat.x * Quat.x * 2;
	float y22 = Quat.y * Quat.y * 2;
	float z22 = Quat.z * Quat.z * 2;

	float xy2 = Quat.x * Quat.y * 2;
	float wz2 = Quat.w * Quat.z * 2;

	float xz2 = Quat.x * Quat.z * 2;
	float wy2 = Quat.w * Quat.y * 2;

	float yz2 = Quat.y * Quat.z * 2;
	float wx2 = Quat.w * Quat.x * 2;


	matrix TransRotObjMatrix = { 1.0 - y22 - z22 , xy2 + wz2 , xz2 - wy2 , 0.0 ,
		xy2 - wz2, 1.0 - x22 - z22 , yz2 + wx2 , 0.0 ,
		xz2 + wy2 , yz2 - wx2, 1.0 - x22 - y22 , 0.0 ,
		InputPosition.x, InputPosition.y, InputPosition.z, 1.0 };

	return TransRotObjMatrix;
}
*/
// Функция умножения позиции объекта на кватернион  без преобразования в матрицу
float3 qtransform(float4 q, float3 v)
{
	return v + 2.0 * cross(cross(v, q.xyz) + q.w * v, q.xyz);
}

GeometryVSOut GeometryVS(GeometryVSIn input, uint VertexNum : SV_VertexID )
{
	GeometryVSOut output;

//	float4 InputPosition = float4(input.position, 1.0f);
	float4 TempPosition = 1.0f;

	TempPosition.xyz = qtransform(input.instanceRotation, input.position.xyz) + input.instancePosition.xyz;

	output.positionView = TempPosition;

	output.position = mul(TempPosition, viewprojection);

	output.normal = normalize(qtransform(input.instanceRotation, input.normal));

	output.texCoord = input.texCoord;
	
	output.viewDirection = normalize(cameraPosition.xyz - TempPosition.xyz);

	return output;
}



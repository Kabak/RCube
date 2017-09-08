////////////////////////////////////////////////////////////////////////////////
// Filename: particle.vs
////////////////////////////////////////////////////////////////////////////////
//#pragma pack_matrix(row_major)

/////////////
// GLOBALS //
/////////////
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


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float4 instancePosition : ObjPOSITION;
	float4 instanceRotation : ObjROTATION;

};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	//float3 tangent : TANGENT;
	float3 viewDirection : TEXCOORD1;
};

// Функция умножения позиции объекта на кватернион  без преобразования в матрицу
float3 qtransform(float4 q, float3 v)
{
	return v + 2.0 * cross(cross(v, q.xyz) + q.w * v, q.xyz);
}

//	http://answers.unity3d.com/questions/416169/finding-pitchrollyaw-from-quaternions.html
//  roll  = Mathf.Atan2(2*y*w - 2*x*z, 1 - 2*y*y - 2*z*z);
//	pitch = Mathf.Atan2(2 * x*w - 2 * y*z, 1 - 2 * x*x - 2 * z*z);
//	yaw   = Mathf.Asin(2 * x*y + 2 * z*w);

/*
// Кватернион в Матрицу 
matrix QuatToMatrix(float4 Quat) {

	float x22 = Quat.x * Quat.x * 2;
	float y22 = Quat.y * Quat.y * 2;
	float z22 = Quat.z * Quat.z * 2;

	float xy2 = Quat.x * Quat.y * 2;
	float wz2 = Quat.w * Quat.z * 2;

	float xz2 = Quat.x * Quat.z * 2;
	float wy2 = Quat.w * Quat.y * 2;

	float yz2 = Quat.y * Quat.z * 2;
	float wx2 = Quat.w * Quat.x * 2;


	matrix Identity = { 1.0 - y22 - z22 , xy2 + wz2 , xz2 - wy2 , 0.0 ,
		xy2 - wz2, 1.0 - x22 - z22 , yz2 + wx2 , 0.0 ,
		xz2 + wy2 , yz2 - wx2, 1.0 - x22 - y22 , 0.0 ,
		0.0, 0.0, 0.0, 1.0 };

	return Identity;

}

// функция для вращения матриц

matrix RotateWorldMatrix(float3 rot)
{
	matrix Identity = { 1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0 };


	matrix rotateX = { 1.0, 0.0, 0.0, 0.0,
		0.0, cos(rot.x), -sin(rot.x), 0.0,
		0.0, sin(rot.x), cos(rot.x), 0.0,
		0.0, 0.0, 0.0, 1.0 };

	matrix rotateY = { cos(rot.y), 0.0, sin(rot.y), 0.0,
		0.0, 1.0, 0.0, 0.0,
		-sin(rot.y), 0.0, cos(rot.y), 0.0,
		0.0, 0.0, 0.0, 1.0 };

	matrix rotateZ = { cos(rot.z), -sin(rot.z), 0.0, 0.0,
		sin(rot.z), cos(rot.z), 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0 };

	return mul(rotateY, mul(rotateX, rotateZ));
}
*/
////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType InstansingVertexShader(VertexInputType input)
{
	PixelInputType output;
	float4 worldPosition;
/*
// Создаём из кватерниона поворота и координат объекта в пространстве матрицу TransRotObjMatrix 
// поворота и положения вертекса
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
		float4 Temp = float4(input.position.xyz, 1.0f);
		output.position = mul(Temp, TransRotObjMatrix);
*/
// Функция умножения позиции объекта на кватернион  без преобразования в матрицу
		output.position = float4(qtransform(input.instanceRotation, input.position.xyz) + input.instancePosition.xyz, 1.0f);
// -------------------------------------------------------------------------------------------------------------------------
		output.position = mul(output.position, viewprojection);
//		output.position = mul(output.position, projectionMatrix);

		output.tex = input.tex;

		// Calculate the normal vector against the world matrix only.
// Заменил на умножение на кватернион
//		output.normal = mul(input.normal, (float3x3)TransRotObjMatrix);
		output.normal = qtransform(input.instanceRotation, input.normal) + input.normal;
// -------------------------------------------------------------------------------------------------------------------------
		// Normalize the normal vector.
		output.normal = normalize(output.normal);

		// Calculate the position of the vertex in the world.
//		worldPosition = mul(input.instancePosition , TransRotObjMatrix);
		worldPosition = input.instancePosition;
		// Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
		output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
		// Normalize the viewing direction vector.
		output.viewDirection = normalize(output.viewDirection);

		return output;
}
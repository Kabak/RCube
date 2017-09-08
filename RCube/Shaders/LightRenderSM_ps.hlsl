cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix viewprojection;
	matrix OrthoMatrix;
	float3 cameraPosition;
	float padding;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXTURE0;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 DepthPixelShader(PixelInputType input) : SV_TARGET
{
	float depthValue;
float4 color;


// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
//depthValue = input.depthPosition.z / input.depthPosition.w;
//color = float4(depthValue, depthValue, depthValue, 1.0f);

depthValue = input.depthPosition.z;
color = float4(input.depthPosition.z, input.depthPosition.z, input.depthPosition.z, 1.0f);
//color = float4(1.0f, 1.0f, 1.0f ,1.0f);
return color;
}
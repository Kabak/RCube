////////////////////////////////////////////////////////////////////////////////
// Filename: particle.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
SamplerState SampleType;


cbuffer LightBuffer : register (b1)
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 diffuseDirection;
	 float DifusePower;
	float4 specularColor;
	float3 specularDirection;
	 float SpecularPower;
/*	float4 PointColor1;
	float4 PointAttenuation1;
	float3 PointPos1;
	 float Power1;

	float4 PointColor2;
	float4 PointAttenuation2;
	float3 PointPos2;
	 float Power2;

	float4 PointColor3;
	float4 PointAttenuation3;
	float3 PointPos3;
	 float Power3;

	float4 PointColor4;
	float4 PointAttenuation4;
	float3 PointPos4;
	 float Power4;
*/
};

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	//float3 tangent : TANGENT;
	float3 viewDirection : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 InstansingPixelShader(PixelInputType input) : SV_TARGET
{

//float4 DifusePower = float4(diffuseColor.w, diffuseColor.w, diffuseColor.w, diffuseColor.w);

//float4 DifusePower = diffuseColor.a;
//float4 SpecularPower = specularColor.a;

float4 textureColor;

float3 DifLightDir;
float DifLightIntensity;
float3 difreflection;

float3 SpecLightDir;
float SpecLightIntensity;
float3 specreflection;
float4 specular = 0.0f;

float4 finalColor;

// Sample the pixel color from the texture using the sampler at this texture coordinate location.
textureColor = shaderTexture.Sample(SampleType, input.tex);



/*

// ������� ������ ����� �������� ����� � �������� �������
float3 lightToPixelVec = PointPos1.xyz - input.position.xyz;
// ������� ���������� ����� ������ � �������� (��� ����� �������)
float d = length(lightToPixelVec);
// ������� ������� (Ambient) ����
float3 finalAmbient = textureColor * light.ambient;

// ���� ������� ������� ������ ����������, ���������� ���� ������� �������� �����
if (d > light.range)
return float4(finalAmbient, diffuse.a);

// ������ lightToPixelVec ��������� ��������, �������� ����������� �������� �� ��������� ������� �����
lightToPixelVec /= d;

// ������ ������������� ����� � ����������� �� ���� � �����������
float howMuchLight = dot(lightToPixelVec, input.normal);

// ���� ���� �� �������� �����������
if (howMuchLight > 0.0f)
{
	// ��������� ��������� � finalColor
	finalColor += howMuchLight * diffuse * light.diffuse;

	// ��������� ������ ���������
	finalColor /= light.att[0] + (light.att[1] * d) + (light.att[2] * (d*d));
}

// ����������, ��� ��������� �� 1 �� 0, � ��������� ������� ��������� 
finalColor = saturate(finalColor + finalAmbient);

// ���������� ������������ ����
return float4(finalColor, diffuse.a);

*/


finalColor = ambientColor;

DifLightDir = -diffuseDirection;
DifLightIntensity = saturate(dot(input.normal, DifLightDir));

SpecLightDir = -specularDirection;
SpecLightIntensity = saturate(dot(input.normal, SpecLightDir));


if (DifLightIntensity > 0.0f)
{
	finalColor += (diffuseColor * DifusePower * DifLightIntensity);
//	finalColor = normalize(finalColor);
}

if (SpecLightIntensity > 0.0f)
{
//	finalColor += (specularColor * SpecularPower * SpecLightIntensity);

	specreflection = normalize(2 * SpecLightIntensity * input.normal - SpecLightDir);

	specular = pow(saturate(dot(specreflection, input.viewDirection)), (1.0f / SpecularPower));
}

finalColor = saturate(finalColor * textureColor);

finalColor = saturate(finalColor + specular);

return finalColor;
}
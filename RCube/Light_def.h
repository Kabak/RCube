#pragma once
#include "DirectXMath.h"

static const int MAX_LIGHTS = 4096;
static const int MAX_LIGHTS_WITH_SHADOW = 4096;

struct PointLight
{
	XMFLOAT3 position;			// ������� ��������� ����� �� �����
	   float attenuationBegin;	// ������ ������ ������� �����
	XMFLOAT3 color;				// ���� ��������
	   float attenuationEnd;	// ������ ��������� ������� �����
	XMFLOAT3 direction;			// ����������� �����, ���� ���� - SpotLight
	   float angel;				// ���� ��������, ���� ���� - SpotLight   ( ���� ���� ����� 0, �� ���� - PointLight )
  XMFLOAT4X4 qtwvp;				// ������� ViewProjection �����
								//		XMFLOAT4 RotQuat;			// ���������� ��������
	bool HaveShadow;			// ����� �� ���� ����
	float ShadowMapSliceNumber;	// ����� �������� ShadowMap � ������� Shadow Maps
	int LightID;				// ����� ����� � ������� ������ ( ����� ��� ��������� ������������� Shadow Map �� ���� ������ � ������
	int Dummy;					// �������� ��� �������� https://developer.nvidia.com/pc-gpu-performance-hot-spots   ��������� ��� �������� ������������ ������� ����� � ������� ������ � ������������ � �������
};


// ��������� ���������� Clustering Lite ��������
struct UIConstants
{
	UINT lightingOnly;
	UINT shadowsOn;
	UINT clusteredGridScale;
	UINT softshadowsOn;
	float PCF_Amount;
	float PCF_Step;
	float ShadowCLAMP;
	float Shadow_Divider;
};


struct PerFrameConstants
{
	// ������ ���������� � ������ �����
	XMFLOAT4 AmbientColor;			// r,g,b,  a - �� ������������
	XMFLOAT4 DiffuseColor;			// r,g,b,  a - ���� �������� 0 - 1 - max
	XMFLOAT3 DiffuseDirection;
	float DiffusePower;
	XMFLOAT4 SpecularColor;			// r,g,b,  a - ���� �������� �� 0 �� 1 - max
	XMFLOAT3 SpecularDirection;
	float SpecularPower;

	XMVECTOR mCameraNearFar;

	unsigned int mFramebufferDimensionsX;
	unsigned int mFramebufferDimensionsY;
	unsigned int mFramebufferDimensionsZ;
	unsigned int mFramebufferDimensionsW;

	UIConstants mUI;
};


// Flat framebuffer RGBA16-encoded
struct FramebufferFlatElement
{
	unsigned int rb;
	unsigned int ga;
};


enum LightCullTechnique
{
	CULL_FORWARD_NONE = 0,
	CULL_FORWARD_PREZ_NONE,
	CULL_DEFERRED_NONE,
	CULL_QUAD,
	CULL_QUAD_DEFERRED_LIGHTING,
	CULL_COMPUTE_SHADER_TILE,
	CULL_CLUSTERED,
};

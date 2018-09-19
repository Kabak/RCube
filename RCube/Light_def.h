#pragma once
#include "DirectXMath.h"

static const int MAX_LIGHTS = 4096;
static const int MAX_LIGHTS_WITH_SHADOW = 4096;

struct PointLight
{
	XMFLOAT3 position;			// Позиция источника света на сцене
	   float attenuationBegin;	// размер начала границы света
	XMFLOAT3 color;				// Цвет свечения
	   float attenuationEnd;	// размер окончания границы света
	XMFLOAT3 direction;			// Направление света, если свет - SpotLight
	   float angel;				// Угол свечения, если свет - SpotLight   ( Если угол равен 0, то свет - PointLight )
  XMFLOAT4X4 qtwvp;				// Матрица ViewProjection света
								//		XMFLOAT4 RotQuat;			// Кватернион поворота
	bool HaveShadow;			// Имеет ли свет тень
	float ShadowMapSliceNumber;	// Номер текстуры ShadowMap в массиве Shadow Maps
	int LightID;				// Номер света в массиве светов ( нужно для наложения множественных Shadow Map от всех светов с тенями
	int Dummy;					// Пустышка для скорости https://developer.nvidia.com/pc-gpu-performance-hot-spots   Использую для хранения назначенного индекса света в массиве светов и привязанного к частице
};


// Структура управления Clustering Lite шейдером
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
	// Список переменных в буфере света
	XMFLOAT4 AmbientColor;			// r,g,b,  a - не используется
	XMFLOAT4 DiffuseColor;			// r,g,b,  a - Сила свечения 0 - 1 - max
	XMFLOAT3 DiffuseDirection;
	float DiffusePower;
	XMFLOAT4 SpecularColor;			// r,g,b,  a - Сила свечения от 0 до 1 - max
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

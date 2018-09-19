#pragma once
#include "stdafx.h"
#include "Material.h"


Material::Material()
{
	Name = nullptr;
	BaseColourTexture = nullptr;
	RoughnessTexture = nullptr;
	MetalicTexture = nullptr;
	SpecularTexture = nullptr;
};


Material::~Material()
{
	RCUBE_ARR_DELETE ( Name )
	RCUBE_RELEASE ( BaseColourTexture )
	RCUBE_RELEASE ( RoughnessTexture )
	RCUBE_RELEASE ( MetalicTexture )
	RCUBE_RELEASE ( SpecularTexture )
};

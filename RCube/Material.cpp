#pragma once
#include "stdafx.h"
#include "Material.h"


Material::Material()
{
	Name = nullptr;
	Texture = nullptr;
};


Material::~Material()
{
	RCUBE_ARR_DELETE ( Name )
	RCUBE_RELEASE ( Texture )
};
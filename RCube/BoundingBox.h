#pragma once
#include "stdafx.h"

struct BoundingBox
{
	void CreateBoundStruct ( RCube_VecFloat234 minValues, RCube_VecFloat234 maxValues );

	RCube_VecFloat234 MinValues; // минимальные значения XYZ ограничивающие коробку
	RCube_VecFloat234 MaxValues; // максимальные значения XYZ ограничивающие коробку

	float SphereRadius; // радиус сферы ограничивающей обьект

	RCube_VecFloat234 CentralPoint; // позиция центральной точки для легкого расчета фраттрума
};

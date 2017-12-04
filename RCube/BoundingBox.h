#pragma once
#include "stdafx.h"

struct BoundingBox
{
	void CreateBoundStruct ( RCube_VecFloat34 minValues, RCube_VecFloat34 maxValues );

	RCube_VecFloat34 MinValues; // минимальные значения XYZ ограничивающие коробку
	RCube_VecFloat34 MaxValues; // максимальные значения XYZ ограничивающие коробку

	float SphereRadius; // радиус сферы ограничивающей обьект

	RCube_VecFloat34 CentralPoint; // позиция центральной точки для легкого расчета фраттрума
};

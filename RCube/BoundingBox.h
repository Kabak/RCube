#pragma once
#include "stdafx.h"

struct BoundingBox
{
	void CreateBoundStruct ( RCube_VecFloat34 minValues, RCube_VecFloat34 maxValues );

	RCube_VecFloat34 MinValues; // ����������� �������� XYZ �������������� �������
	RCube_VecFloat34 MaxValues; // ������������ �������� XYZ �������������� �������

	float SphereRadius; // ������ ����� �������������� ������

	RCube_VecFloat34 CentralPoint; // ������� ����������� ����� ��� ������� ������� ���������
};

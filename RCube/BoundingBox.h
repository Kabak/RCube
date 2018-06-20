#pragma once
#include "stdafx.h"

struct BoundingBox
{
	void CreateBoundStruct ( RCube_VecFloat234 minValues, RCube_VecFloat234 maxValues );

	RCube_VecFloat234 MinValues; // ����������� �������� XYZ �������������� �������
	RCube_VecFloat234 MaxValues; // ������������ �������� XYZ �������������� �������

	float SphereRadius; // ������ ����� �������������� ������

	RCube_VecFloat234 CentralPoint; // ������� ����������� ����� ��� ������� ������� ���������
};

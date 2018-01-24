#pragma once
#include "stdafx.h"

#ifndef _CUBEMAP_H_
#define _CUBEMAP_H_

class CubeMap
{
public:
	CubeMap ();
   ~CubeMap ();

	int CubeMapTextureIndex;	// ����� ��������
	int CubeMapBuffersIndex;	// ����� ������� 
	int ShaderIndex;			// ����� ������� ��� ��������� CubMap
	int NumSphereFaces;			// ���������� ���������� � �����

	WCHAR* CubeMapTextureName;	// ��� ��������

};

#endif
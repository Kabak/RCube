#pragma once
#include "stdafx.h"

#ifndef _CUBEMAP_H_
#define _CUBEMAP_H_

class CubeMap
{
public:
	CubeMap ();
   ~CubeMap ();

	int CubeMapTextureIndex;	// Номер текстуры
	int CubeMapBuffersIndex;	// Номер буферов 
	int ShaderIndex;			// Номер шейдера для рисования CubMap
	int NumSphereFaces;			// Количество примитивов в сфере

	WCHAR* CubeMapTextureName;	// Имя текстуры

};

#endif
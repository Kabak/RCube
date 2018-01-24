#include "stdafx.h"
#include "CubeMap.h"

CubeMap::CubeMap ( )
{
	CubeMapTextureIndex = -1;
	CubeMapBuffersIndex = -1;
	CubeMapTextureName = nullptr;
	NumSphereFaces = 0;
}


CubeMap::~CubeMap ()
{

}
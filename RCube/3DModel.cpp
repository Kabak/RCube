#include "stdafx.h"
#include "3DModel.h"

using namespace DirectX;


_3DModel::_3DModel ()
{
	Positions = nullptr;
	PositionsDraw = nullptr;
	InstancePositionsBuffer = nullptr;
}


_3DModel::~_3DModel ()
{
	
	RCUBE_DELETE ( InstancePositionsBuffer );
/*
	size_t c = Meshes.size ();
	for (size_t i = 0; i < c; ++i)
	{
		RCUBE_DELETE ( Meshes[i] )
	}
*/
	Meshes.clear ();

	RCUBE_ARR_DELETE ( Positions );
	RCUBE_ARR_DELETE ( PositionsDraw );
	
}
#include "stdafx.h"

#include "Terrain.h"

Terrain::Terrain ()
{
	VB_Data = nullptr;
	IB_Data = nullptr;
	InstancePositionsBuffer = nullptr;
};


Terrain::~Terrain ()
{
	RCUBE_ARR_DELETE ( VB_Data );
	RCUBE_ARR_DELETE ( IB_Data );
	RCUBE_DELETE ( InstancePositionsBuffer );
};
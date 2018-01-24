#include "stdafx.h"

#include "Terrain.h"

Terrain::Terrain ()
{
	m_heightMap = nullptr;
	VB_Data = nullptr;
	IB_Data = nullptr;
	InstancePositionsBuffer = nullptr;
};


Terrain::~Terrain ()
{
	RCUBE_ARR_DELETE ( m_heightMap );
	RCUBE_ARR_DELETE ( VB_Data );
	RCUBE_ARR_DELETE ( IB_Data );
	RCUBE_DELETE ( InstancePositionsBuffer );
};
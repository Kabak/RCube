#include "stdafx.h"
#include "IndexesArray.h"


IndexseArray::IndexseArray()
{
	 EleventsAmount = 0;
		LastElement = 0;

			  Array = nullptr;
		
		 Camera.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };
		 Object.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };

}

IndexseArray::~IndexseArray()
{
	void Shutdown();
}


void IndexseArray::Shutdown()
{
	RCUBE_ARR_DELETE( Array )
}


bool IndexseArray::InitArray( int Amount, bool FartoCamFirst )
{
	Array = new int [ Amount ];

	if ( Array )
	{
//		memset( FireInstIndNumber, 0xffffffff, sizeof( UINT ) * MaxParticles );
		return true;
	}

	return false;
}


template <class T> void IndexseArray::QuickDepthSortAscending( T* indices, float* depths, int lo, int hi )
{
	//http://kvodo.ru/quicksort.html
	//  lo is the lower index, hi is the upper index
	//  of the region of array a that is to be sorted
	int i = lo, j = hi;
	float h;
	T index;
	float x = depths[( lo + hi ) / 2];

	//  partition
	do
	{
		while ( depths[i] < x ) i++;
		while ( depths[j] > x ) j--;
		if ( i <= j )
		{
			h = depths[i]; depths[i] = depths[j]; depths[j] = h;
			index = indices[i]; indices[i] = indices[j]; indices[j] = index;
			i++; j--;
		}
	}
	while ( i <= j );

	//  recursion
	if ( lo < j ) QuickDepthSortAscending( indices, depths, lo, j );
	if ( i < hi ) QuickDepthSortAscending( indices, depths, i, hi );


}


template <class T> void IndexseArray::QuickDepthSortDescending( T* indices, float* depths, int lo, int hi )
{
	//http://kvodo.ru/quicksort.html
	//  lo is the lower index, hi is the upper index
	//  of the region of array a that is to be sorted
	int i = lo, j = hi;
	float h;
	T index;
	float x = depths[( lo + hi ) / 2];

	//  partition
	do
	{
		while ( depths[i] > x ) i++;
		while ( depths[j] < x ) j--;
		if ( i <= j )
		{
			h = depths[j]; depths[j] = depths[i]; depths[i] = h;
			index = indices[j]; indices[j] = indices[i]; indices[i] = index;
			i++; j--;
		}
	}
	while ( i <= j );

	//  recursion
	if ( lo < j ) QuickDepthSortDescending( indices, depths, lo, j );
	if ( i < hi ) QuickDepthSortDescending( indices, depths, i, hi );


}


void IndexseArray::DistanceCalk( XMFLOAT3 &ObjPos, XMFLOAT3 &Camerapos, float &DistanceStore )
{
	// http://stackoverflow.com/questions/10291862/what-is-the-best-way-to-get-distance-between-2-points-with-directxmath

	RCube_VecFloat234 Result;

	Camera.Fl3 = Camerapos;
	Object.Fl3 = ObjPos;

	Result.Vec = XMVector3Length( XMVectorSubtract( Camera.Vec, Object.Vec ) );

	if ( Result.Fl3.x < 0.0f )
		DistanceStore = -Result.Fl3.x;
	else
		DistanceStore = Result.Fl3.x;
}
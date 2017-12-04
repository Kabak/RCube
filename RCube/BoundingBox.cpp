#include "stdafx.h"
#include "BoundingBox.h"


void BoundingBox::CreateBoundStruct ( RCube_VecFloat34 minValues, RCube_VecFloat34 maxValues ) {

	MinValues.Fl4 = minValues.Fl4;
	MaxValues.Fl4 = maxValues.Fl4;


	CentralPoint.Vec = (MaxValues.Vec + MinValues.Vec) / 2;


	SphereRadius = sqrt ( pow ( MaxValues.Fl3.x - MinValues.Fl3.x, 2 ) + pow ( MaxValues.Fl3.y - MinValues.Fl3.y, 2 ) + pow ( MaxValues.Fl3.z - MinValues.Fl3.z, 2 ) );
}
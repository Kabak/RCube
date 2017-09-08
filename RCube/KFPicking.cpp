#include "stdafx.h"
#include "KFPicking.h"

HRESULT KFPicking::Init(HWND hwnd , int WindowPosX , int WindowPosY , int ScreenWidth , int ScreenHeight 
						, KF3DObjectData ObjArray[] , int NumOfObjects){

	// расчет оси в которой лежит правая граница окна
	g_ScreenWidth = ScreenWidth ;
	//расчет оси в которой лежит нижняя граница окна
	g_ScreenHeight = ScreenHeight ;

	g_WindowPosX = WindowPosX ;
	g_WindowPosY = WindowPosY ;

	ObjDataArray = new KF3DObjectData[NumOfObjects] ;

	int c =0 ;
	while (c < NumOfObjects){
	      ObjDataArray[c] = ObjArray[c] ;
		  ++c ;
	}

	ObjectNum = NumOfObjects ;

	AproofObjectsDesc = new bool[ObjectNum] ;

	c =0 ;
	while (c < ObjectNum){
		AproofObjectsDesc[c] = true ;
		++c ;
	}

	return S_OK ;
}

void KFPicking::pickRayVector(float mouseX, float mouseY, XMVECTOR& pickRayInWorldSpacePos, XMVECTOR& pickRayInWorldSpaceDir , 
							  XMMATRIX & camProjection , XMMATRIX & camView)
{
	XMVECTOR pickRayInViewSpaceDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR pickRayInViewSpacePos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	float PRVecX, PRVecY, PRVecZ;

	//Transform 2D pick position on screen space to 3D ray in View space
	PRVecX =  ((( 2.0f * mouseX) / g_ScreenWidth ) - 1) / camProjection.r[0].m128_f32[0];
	PRVecY = -((( 2.0f * mouseY) / g_ScreenHeight) - 1) / camProjection.r[1].m128_f32[1];
	PRVecZ =  1.0f;	//View space's Z direction ranges from 0 to 1, so we set 1 since the ray goes "into" the screen

	pickRayInViewSpaceDir = XMVectorSet(PRVecX, PRVecY, PRVecZ, 0.0f);

	//Uncomment this line if you want to use the center of the screen (client area)
	//to be the point that creates the picking ray (eg. first person shooter)
	//pickRayInViewSpaceDir = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	// Transform 3D Ray from View space to 3D ray in World space
	XMMATRIX pickRayToWorldSpaceMatrix;
	XMVECTOR matInvDeter;	//We don't use this, but the xna matrix inverse function requires the first parameter to not be null

	pickRayToWorldSpaceMatrix = XMMatrixInverse(&matInvDeter, camView);	//Inverse of View Space matrix is World space matrix

	pickRayInWorldSpacePos = XMVector3TransformCoord(pickRayInViewSpacePos, pickRayToWorldSpaceMatrix);
	pickRayInWorldSpaceDir = XMVector3TransformNormal(pickRayInViewSpaceDir, pickRayToWorldSpaceMatrix);
}

float KFPicking::pick(XMVECTOR pickRayInWorldSpacePos,
	XMVECTOR pickRayInWorldSpaceDir, 
	std::vector<XMFLOAT3>& vertPosArray,
	std::vector<DWORD>& indexPosArray, 
	XMMATRIX& worldSpace)
{ 		
	float Result = FLT_MAX ;
	float DistOfLastTriangle = -1.0f ;
	//Loop through each triangle in the object
	for(unsigned int i = 0; i < indexPosArray.size()/3; ++i)
	{
		//Triangle's vertices V1, V2, V3
		XMVECTOR tri1V1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR tri1V2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR tri1V3 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		//Temporary 3d floats for each vertex
		XMFLOAT3 tV1, tV2, tV3;

		//Get triangle 
		tV1 = vertPosArray[indexPosArray[(i*3)+0]];
		tV2 = vertPosArray[indexPosArray[(i*3)+1]];
		tV3 = vertPosArray[indexPosArray[(i*3)+2]];

		tri1V1 = XMVectorSet(tV1.x, tV1.y, tV1.z, 0.0f);
		tri1V2 = XMVectorSet(tV2.x, tV2.y, tV2.z, 0.0f);
		tri1V3 = XMVectorSet(tV3.x, tV3.y, tV3.z, 0.0f);

		//Transform the vertices to world space
		tri1V1 = XMVector3TransformCoord(tri1V1, worldSpace);
		tri1V2 = XMVector3TransformCoord(tri1V2, worldSpace);
		tri1V3 = XMVector3TransformCoord(tri1V3, worldSpace);

		//Find the normal using U, V coordinates (two edges)
		XMVECTOR U = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR V = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR faceNormal = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		U = tri1V2 - tri1V1;
		V = tri1V3 - tri1V1;

		//Compute face normal by crossing U, V
		faceNormal = XMVector3Cross(U, V);

		faceNormal = XMVector3Normalize(faceNormal);

		//Calculate a point on the triangle for the plane equation
		XMVECTOR triPoint = tri1V1;

		//Get plane equation ("Ax + By + Cz + D = 0") Variables
		float tri1A = XMVectorGetX(faceNormal);
		float tri1B = XMVectorGetY(faceNormal);
		float tri1C = XMVectorGetZ(faceNormal);
		float tri1D = (-tri1A*XMVectorGetX(triPoint) - tri1B*XMVectorGetY(triPoint) - tri1C*XMVectorGetZ(triPoint));

		//Now we find where (on the ray) the ray intersects with the triangles plane
		float ep1, ep2, t = 0.0f;
		float planeIntersectX, planeIntersectY, planeIntersectZ = 0.0f;
		XMVECTOR pointInPlane = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		ep1 = (XMVectorGetX(pickRayInWorldSpacePos) * tri1A) + (XMVectorGetY(pickRayInWorldSpacePos) * tri1B) + (XMVectorGetZ(pickRayInWorldSpacePos) * tri1C);
		ep2 = (XMVectorGetX(pickRayInWorldSpaceDir) * tri1A) + (XMVectorGetY(pickRayInWorldSpaceDir) * tri1B) + (XMVectorGetZ(pickRayInWorldSpaceDir) * tri1C);

		//Make sure there are no divide-by-zeros
		if(ep2 != 0.0f)
			t = -(ep1 + tri1D)/(ep2);

		if(t > 0.0f)    //Make sure you don't pick objects behind the camera
		{
			//Get the point on the plane
			planeIntersectX = XMVectorGetX(pickRayInWorldSpacePos) + XMVectorGetX(pickRayInWorldSpaceDir) * t;
			planeIntersectY = XMVectorGetY(pickRayInWorldSpacePos) + XMVectorGetY(pickRayInWorldSpaceDir) * t;
			planeIntersectZ = XMVectorGetZ(pickRayInWorldSpacePos) + XMVectorGetZ(pickRayInWorldSpaceDir) * t;

			pointInPlane = XMVectorSet(planeIntersectX, planeIntersectY, planeIntersectZ, 0.0f);

			//Call function to check if point is in the triangle
			if( PointInTriangle(tri1V1, tri1V2, tri1V3, pointInPlane) )
			{
				if (t/2.0f < DistOfLastTriangle || DistOfLastTriangle == -1){
					AproofTriangeleIndex = i ;
					DistOfLastTriangle  = t/2.0f ;
					Result = t/2.0f;
				}
				//Return the distance to the hit, so you can check all the other pickable objects in your scene
				//and choose whichever object is closest to the camera
			}
		}
	}
	//return the max float value (near infinity) if an object was not picked
	return Result;
}

bool KFPicking::PointInTriangle(XMVECTOR& triV1, XMVECTOR& triV2, XMVECTOR& triV3, XMVECTOR& point )
{
	//To find out if the point is inside the triangle, we will check to see if the point
	//is on the correct side of each of the triangles edges.

	XMVECTOR cp1 = XMVector3Cross((triV3 - triV2), (point - triV2));
	XMVECTOR cp2 = XMVector3Cross((triV3 - triV2), (triV1 - triV2));
	if(XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
	{
		cp1 = XMVector3Cross((triV3 - triV1), (point - triV1));
		cp2 = XMVector3Cross((triV3 - triV1), (triV2 - triV1));
		if(XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
		{
			cp1 = XMVector3Cross((triV2 - triV1), (point - triV1));
			cp2 = XMVector3Cross((triV2 - triV1), (triV3 - triV1));
			if(XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
			{
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}
	return false;
}

int KFPicking::ApproofObjs(int X , int Y , XMMATRIX& Projection , XMMATRIX& View , int & TriangleIndex){
	// если ничего не найдтся то вернёт -1
	int result = -1 ;

	int mousex = X - g_WindowPosX;
	int mousey = Y - g_WindowPosY;
	float tempDist;

    XMVECTOR prwsPos, prwsDir;
    float closestDist = FLT_MAX;
	pickRayVector((float)mousex , (float)mousey , prwsPos , prwsDir , Projection , View ) ;

	int c= 0 ;
	// пока с <  количества обьектов
	while (c < ObjectNum)
	{
		if(AproofObjectsDesc[c]){
         // Transform 3D Ray from View space to 3D ray in World space
		tempDist = pick(prwsPos, prwsDir, ObjDataArray[c].ObjVertPosArray , ObjDataArray[c].ObjVertIndexArray , *ObjDataArray[c].ObjWorld );
		if(tempDist < closestDist)
			{
				closestDist = tempDist;
				result = c ;
				TriangleIndex = AproofTriangeleIndex ;
			}
		}
		++c ;
	}

	return result ;

}

KFPicking::~KFPicking(){

	delete [] ObjDataArray ;
	delete [] AproofObjectsDesc ;

}
#include "stdafx.h"
////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cameraclass.h"

// The class constructor will initialize the position and rotation of the camera to be at the origin of the scene.

CameraClass::CameraClass()
{
// +++++++++++   настройки камеры по умолчанию   ++++++
	DefaultForward = XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
	  DefaultRight = XMVectorSet(1.0f,0.0f,0.0f, 0.0f);
		 DefaultUp = XMVectorSet(0.0f,1.0f,0.0f, 0.0f);
	    camForward = XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
	      camRight = XMVectorSet(1.0f,0.0f,0.0f, 0.0f);
			 camUp = XMVectorSet(0.0f,1.0f,0.0f, 0.0f);

//	            up = XMVectorSet(0.0f,1.0f,0.0f, 0.0f);
	      position = XMVectorSet(0.0f,0.0f,0.0f, 0.0f);
		    lookAt = XMVectorSet(0.0f,0.0f,0.000001f, 0.0f);//0.0000004f,0.0f,0.000001f,0.0f); ќ„≈Ќ№ „”¬—“¬»“≈Ћ№Ќјя ’–≈Ќ№ !!!
			previos_position = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
			previos_lookAt = XMVectorSet(0.0f, 0.0f, 0.000001f, 0.0f);

	FirstPersonCam = true;
	IsItAnyCameraReplase = true;

     moveLeftRight = 0.0f;
   moveBackForward = 0.0f;
			moveUp = 0.0f;
// ----------------------------------------------------
	  m_position.x = 0.0f;
	  m_position.y = 0.0f;
	  m_position.z = 0.0f;

	  m_rotation.x = 0.0f;
	  m_rotation.y = 0.0f;
	  m_rotation.z = 0.0f;

	PitchYawRoll.x = 0.0f;
	PitchYawRoll.y = 0.0f;
	PitchYawRoll.z = 0.0f;

	  m_viewMatrix = XMMatrixIdentity();
	RotationMatrix = XMMatrixIdentity();
		 CamMatrix = XMMatrixIdentity();
 RotateYTempMatrix = XMMatrixIdentity();
 RotateXTempMatrix = XMMatrixIdentity();
 RotateZTempMatrix = XMMatrixIdentity();
}


CameraClass::CameraClass(const CameraClass &other)
{
}


CameraClass::~CameraClass()
{
}


void CameraClass::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
	position = XMLoadFloat3( &m_position );
}


void CameraClass::SetRotation(float x, float y, float z)
{
	FirstPersonCam = false;
	PitchYawRoll.x = m_rotation.x = x;
	PitchYawRoll.y = m_rotation.y = y;
	PitchYawRoll.z = m_rotation.z = z;	
}


void CameraClass::SetRotationFirstPerson(float x, float y, float z)
{
	FirstPersonCam = true;
	PitchYawRoll.x = m_rotation.x = x;
	PitchYawRoll.y = m_rotation.y = y;
	PitchYawRoll.z = m_rotation.z = z;
}


void CameraClass::GetRPY( float&P, float&Y, float&R )
{
	P = PitchYawRoll.x;
	Y = PitchYawRoll.y;
	R = PitchYawRoll.z;
}


void CameraClass::SetLookAt(float x, float y, float z)
{
	FirstPersonCam = true;
	lookAt = XMVectorSet(x,y,z,0.0f);	

// http://stackoverflow.com/questions/1251828/calculate-rotations-to-look-at-a-3d-point
// http://www.euclideanspace.com/maths/algebra/vectors/lookat/

	float _x = x - m_position.x;
	float _y = y - m_position.y;
	float _z = z - m_position.z;

	PitchYawRoll.x = m_rotation.x = -atan2( _y, sqrt( _x*_x + _z*_z ) );
	PitchYawRoll.y = m_rotation.x = atan2( _x, _z );
	PitchYawRoll.z = m_rotation.x = 0.0f;

}


void CameraClass::SetUpPosition(float x, float y, float z)
{
	camUp = XMVectorSet(x,y,z,0.0f);	
}


void CameraClass::GetPosition(XMFLOAT3 &Out)
{
	Out = m_position; 
}


void CameraClass::GetLookAt( XMVECTOR &Out )
{
	Out = lookAt;
}


 void CameraClass::GetRotation(XMFLOAT3 &Out)
{
	Out = m_rotation;
}

// The Render function uses the position and rotation of the camera to build and update the view matrix. We first setup our variables for up, position, rotation, and so forth. 
// Then at the origin of the scene we first rotate the camera based on the x, y, and z rotation of the camera. Once it is properly rotated when then translate the camera to the position in 3D space.
// With the correct values in the position, lookAt, and up we can then use the D3DXMatrixLookAtLH function to create the view matrix to represent the current camera rotation and translation.

void CameraClass::Render()
{
	RotationMatrix = XMMatrixRotationRollPitchYaw(PitchYawRoll.x,PitchYawRoll.y,PitchYawRoll.z);

	if ( FirstPersonCam == false )
	{
	// Create the rotation matrix from the yaw, pitch, and roll values.
			 camUp = XMVector3TransformCoord(camUp,RotationMatrix);
			lookAt = XMVector3TransformCoord(lookAt,RotationMatrix);
		  position = XMVector3TransformCoord(position,RotationMatrix);

	// Finally create the view matrix from the three updated vectors.
	//m_viewMatrix = XMMatrixLookAtLH(position,lookAt,up);
	}
	else
	{

	lookAt = XMVector3TransformCoord(DefaultForward, RotationMatrix);
//	lookAt = XMVector3Normalize(lookAt);


//	RotateYTempMatrix = XMMatrixRotationY(PitchYawRoll.y);
//	RotateXTempMatrix = XMMatrixRotationX(PitchYawRoll.x);
//	RotateZTempMatrix = XMMatrixRotationX(PitchYawRoll.z);


//	camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
//	camUp = XMVector3TransformCoord(DefaultUp, RotateYTempMatrix);
//	camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);
	camRight = XMVector3TransformCoord(DefaultRight, RotationMatrix); // * RotateXTempMatrix * RotateZTempMatrix
	camUp = XMVector3TransformCoord(DefaultUp, RotationMatrix); //* RotateXTempMatrix * RotateZTempMatrix
	camForward = XMVector3TransformCoord(DefaultForward, RotationMatrix );// * RotateXTempMatrix * RotateZTempMatrix

	position += moveLeftRight*camRight;
	position += moveBackForward*camForward;
	position += moveUp*camUp;
// ќбновл€ем позицию камеры дл€ запросов позиции камеры внешними классами
	XMStoreFloat3(&m_position, position);

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;
	moveUp = 0.0f;

	lookAt = position + lookAt;	
//	camUp = position + camUp;

	}
	
	m_viewMatrix = XMMatrixLookAtLH( position, lookAt, camUp );

	IsItAnyCameraReplase = false;

	if (!XMVector3Equal(position, previos_position) || !XMVector3Equal(lookAt, previos_lookAt)) {

		IsItAnyCameraReplase = true;
		previos_position = position;
		previos_lookAt = lookAt;

	}

	return;
}

// After the Render function has been called to create the view matrix we can provide the updated view matrix to calling functions using this GetViewMatrix function. 
// The view matrix will be one of the three main matrices used in the HLSL vertex shader.

void CameraClass::GetViewMatrix(XMMATRIX &viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}


void CameraClass::SetRotationMatrix(XMMATRIX& rotMatrix)
{
	RotationMatrix = rotMatrix;
}


void CameraClass::SetDefaultView()
{
// RotationMatrix = XMMatrixIdentity();
		 lookAt = XMVector3TransformCoord( DefaultForward, RotationMatrix );
	      camUp = XMVectorSet(0.0f,1.0f,0.0f,0.0f);
       camRight = XMVectorSet(1.0f,0.0f,0.0f,0.0f);
     camForward = XMVectorSet(0.0f,0.0f,1.0f,0.0f);
  moveLeftRight = 0.0f;
moveBackForward = 0.0f;
SetRotation(0.0f,0.0f,0.0f);
// SetPosition(0.0f,0.0f,-20.0f);
//	lookAt = XMVector3Normalize(lookAt);

}


void CameraClass::AroundXYZ(const float &Angel,const UINT &Axis){

	float X,Y,Z, ANcos,ANsin;

		ANcos = (float)cos (Angel);
		ANsin = (float)sin (Angel);

	if ( Axis == _Y ){
// X
		X = VectorX.X * ANcos - VectorX.Z * ANsin;
		Z = VectorX.X * ANsin + VectorX.Z * ANcos;

		VectorX.X = X;
		VectorX.Z = Z;

// Y
		X = VectorY.X * ANcos - VectorY.Z * ANsin;
		Z = VectorY.X * ANsin + VectorY.Z * ANcos;

		VectorY.X = X;
		VectorY.Z = Z;


// Z
		X = VectorZ.X * ANcos - VectorZ.Z * ANsin;
		Z = VectorZ.X * ANsin + VectorZ.Z * ANcos; 

		VectorZ.X = X;
		VectorZ.Z = Z;

	}
	if ( Axis == _X ){
// X
		Y = VectorX.Y * ANcos - VectorX.Z * ANsin;
		Z = VectorX.Y * ANsin + VectorX.Z * ANcos;

		VectorX.Y = Y;
		VectorX.Z = Z;
		
// Y
		Y = VectorY.Y * ANcos - VectorY.Z * ANsin;
		Z = VectorY.Y * ANsin + VectorY.Z * ANcos;
		
		VectorY.Y = Y;
		VectorY.Z = Z;
// Z
		Y = VectorZ.Y * ANcos - VectorZ.Z * ANsin;
		Z = VectorZ.Y * ANsin + VectorZ.Z * ANcos; 

		VectorZ.Y = Y;
		VectorZ.Z = Z;

	}
}


void CameraClass::CameraRotate(XMFLOAT3& axis, float degrees)
{
	if (XMVector3Equal(GMathFV(axis), XMVectorZero()) ||
		degrees == 0.0f)
		return;

	XMVECTOR lookAtVector = XMVector3Transform( lookAt - position, 
		XMMatrixRotationNormal(GMathFV(axis), XMConvertToRadians(degrees)));
	XMVECTOR camUpVector = XMVector3Transform( camUp - position,
		XMMatrixRotationNormal(GMathFV(axis), XMConvertToRadians(degrees)));
	lookAt = position + lookAtVector;
	camUp = position + camUpVector;
	m_viewMatrix -= XMMatrixLookAtLH(position, lookAt, camUp);
}


void CameraClass::CameraMove(XMFLOAT3& direction)
{
	position = XMVector3Transform(position,
		XMMatrixTranslation(direction.x, direction.y, direction.z));
	lookAt = XMVector3Transform(lookAt,
		XMMatrixTranslation(direction.x, direction.y, direction.z));
	camUp = XMVector3Transform(camUp,
		XMMatrixTranslation(direction.x, direction.y, direction.z));

	m_viewMatrix = XMMatrixLookAtLH(position, lookAt, camUp);
}

void CameraClass::MoveForward(float speed)
{

}


void CameraClass::MoveRight(float speed)
{

}
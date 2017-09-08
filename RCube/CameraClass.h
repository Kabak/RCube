#pragma once
#include "stdafx.h"

#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_

#include "DirectXMath.h"

// http://www.gamedev.net/page/resources/_/technical/directx-and-xna/directx-11-c-game-camera-r2978

//using namespace std;
using namespace DirectX;

//__declspec(align(16)) 
class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
   ~CameraClass();

// First person movement
	void MoveForward( float speed );
	void MoveRight( float speed );

	void CameraRotate( XMFLOAT3& axis, float degrees );
	void CameraMove(XMFLOAT3& direction);
	void SetRotation(float, float, float);
	void SetPosition(float, float, float);
	void SetRotationFirstPerson(float, float, float);
// ���������� ������ ��� ��������� X � ������������ ��������� ~20
	void SetUpPosition(float, float, float);
// 0.0000004f,0.0f,0.000001f,0.0f); ����� �������������� ����� !!!
	void SetLookAt(float, float, float);
	void SetDefaultView();

	void GetPosition(XMFLOAT3&);
	void GetLookAt( XMVECTOR& );
	void GetRPY(float&, float&, float&);
	void GetRotation(XMFLOAT3&);
	void SetRotationMatrix(XMMATRIX&);
	void GetViewMatrix(XMMATRIX&);

	void Render();

	void AroundXYZ(const float &Angel,const UINT &Axis);

// �������������� ��������� New � delete ��� ����������� ���������� XMMATRIX � ������
    void* operator new(size_t i)
    {
        return _mm_malloc(i,16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }
// ------------------------------------
// ������ ���� �� 1-�� ����
	bool FirstPersonCam;

// ������� ����������� ������ �� ���������
XMVECTOR DefaultForward;// XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
XMVECTOR DefaultRight;// XMVectorSet(1.0f,0.0f,0.0f, 0.0f);
XMVECTOR DefaultUp;// XMVectorSet(0.0f,1.0f,0.0f, 0.0f);
XMVECTOR camForward;// XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
XMVECTOR camRight;// XMVectorSet(1.0f,0.0f,0.0f, 0.0f);
XMVECTOR camUp;// XMVectorSet(0.0f,1.0f,0.0f, 0.0f);
XMVECTOR position;
XMVECTOR lookAt;
// ��� �������� ������ �����/�����  �����/������
float moveLeftRight; // ����������� �������� ������ - ����, + �����
float moveBackForward; // ����������� �������� ������ - �����, + �����
float moveUp; // ����������� ����� - ����, + �����
bool IsItAnyCameraReplase;// ���������� �������� �� ������������ ������ � ����������� �����

private :

	inline XMVECTOR GMathFV(XMFLOAT3& val)
	{
		return XMLoadFloat3(&val);
	}

	inline XMFLOAT3 GMathVF(XMVECTOR& vec)
	{
		XMFLOAT3 val;
		XMStoreFloat3(&val, vec);
		return val;
	}


XMFLOAT3 m_position;
XMFLOAT3 m_rotation;

XMFLOAT3 PitchYawRoll;

// ������ ���� �������� ������
enum Axis_XYZ {_X,_Y,_Z};

// ������ ����������� ���� ����� �������� ������
struct Vector {
float X;
float Y;
float Z;
float W;
} VectorX,VectorY,VectorZ;

// ������� �������� ����
XMMATRIX m_viewMatrix;
// ������� �������� ������
XMMATRIX RotationMatrix;
// ������� ����� ������
XMMATRIX CamMatrix;
// ��������� �������
XMMATRIX RotateYTempMatrix;
XMMATRIX RotateXTempMatrix;
XMMATRIX RotateZTempMatrix;
XMVECTOR previos_position;
XMVECTOR previos_lookAt;
};

#endif
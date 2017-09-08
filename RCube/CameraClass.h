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
// Изменяется только при изменении X в определенном диапозоне ~20
	void SetUpPosition(float, float, float);
// 0.0000004f,0.0f,0.000001f,0.0f); ОЧЕНЬ ЧУВСТВИТЕЛЬНАЯ ХРЕНЬ !!!
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

// Переопределяем операторы New и delete для правильного размещения XMMATRIX в памяти
    void* operator new(size_t i)
    {
        return _mm_malloc(i,16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }
// ------------------------------------
// Камера вида от 1-го лица
	bool FirstPersonCam;

// Векторы направления камеры по умолчанию
XMVECTOR DefaultForward;// XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
XMVECTOR DefaultRight;// XMVectorSet(1.0f,0.0f,0.0f, 0.0f);
XMVECTOR DefaultUp;// XMVectorSet(0.0f,1.0f,0.0f, 0.0f);
XMVECTOR camForward;// XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
XMVECTOR camRight;// XMVectorSet(1.0f,0.0f,0.0f, 0.0f);
XMVECTOR camUp;// XMVectorSet(0.0f,1.0f,0.0f, 0.0f);
XMVECTOR position;
XMVECTOR lookAt;
// При движении камеры вперёд/назад  влево/вправо
float moveLeftRight; // направление движения камеры - лево, + право
float moveBackForward; // направление движения камеры - назад, + вперёд
float moveUp; // нфправление полёта - вниз, + вверх
bool IsItAnyCameraReplase;// переменная атвечает за передвижение камеры с преведущего кадра

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

// Список осей вращения камеры
enum Axis_XYZ {_X,_Y,_Z};

// Вектор направления осей после вращения камеры
struct Vector {
float X;
float Y;
float Z;
float W;
} VectorX,VectorY,VectorZ;

// Матрица готового вида
XMMATRIX m_viewMatrix;
// Матрица вращения камеры
XMMATRIX RotationMatrix;
// Матрица самой камеры
XMMATRIX CamMatrix;
// Временная матрица
XMMATRIX RotateYTempMatrix;
XMMATRIX RotateXTempMatrix;
XMMATRIX RotateZTempMatrix;
XMVECTOR previos_position;
XMVECTOR previos_lookAt;
};

#endif
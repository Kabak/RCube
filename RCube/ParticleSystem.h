#pragma once
#include "stdafx.h"
#include <vector>
#include "DirectXMath.h"
#include "Emiters.h"


using namespace DirectX;
using namespace std;

class ParticleSystem
{
public:

	ParticleSystem ();
   ~ParticleSystem ();

   bool FrustumPassed;	// Set if in Frustum
   bool Render;			// Set if System need to be rendered
//    int SystemType;		// Type of Particle system

	int ParticleSystem_Object_Index; // Particle System Index in massive

   RCube_VecFloat234 Position;	// Position in 3D world
   RCube_VecFloat234 Rotation;	// Rotation in 3D world
   RCube_VecFloat234 AABB;		// Dimensions of System for frustum checking

   vector <Emitter*> Emitters;	// Particle system consist of paricles Emitters

private:

};
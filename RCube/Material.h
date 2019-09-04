#pragma once
#include "stdafx.h"
#include "DirectXMath.h"
#include "D3DGlobalContext.h"
#include <vector>
#include <string>

using namespace std;
using namespace DirectX;


struct MaterialOtherData
{
public:
	std::wstring* Name;
	ID3D11ShaderResourceView * BaseColourTexture;
	ID3D11ShaderResourceView * RoughnessTexture;
	ID3D11ShaderResourceView * MetalicTexture;
	ID3D11ShaderResourceView * SpecularTexture;


};


// Materials
// https://www.3dgep.com/texturing-lighting-directx-11/#Materials
// http://paulbourke.net/dataformats/mtl/
struct  MaterialShaderData
{
public:
	XMFLOAT3 Ka;// Ambient light
	float Ni;	// Specifies the optical density for the surface ( index of refraction ) from 0.001 to 10 
				// Glass has an index of refraction of about 1.5.
				// Values of less than 1.0 produce bizarre results and are not recommended.

	XMFLOAT3 Kd;// Diffuse light  
	float d;	// Dissolve for the current material
				// "factor" is the amount this material dissolves into the background.  
				// A factor of 1.0 is fully opaque.This is the default when a new material
				// is created.A factor of 0.0 is fully dissolved( completely transparent ).

	XMFLOAT3 Ke;// Emmisive light amount for Ambient, Diffuse, Specular
	int illum;	// 

	XMFLOAT3 Ks;// Specular amount
	float Ns;	// Specular Exponent 
				// A high exponent results in a tight, concentrated highlight.
				// Ns values normally range from 0 to 1000.

	XMFLOAT3 Tf;
	float Tr;

};


class Material : MaterialOtherData, MaterialShaderData
{
public:
	Material();
   ~Material();

private:
	
};
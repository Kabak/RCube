#pragma once
#include "stdafx.h"

#ifndef _KFRESORSEMANEGER_H_
#define _KFRESORSEMANEGER_H_

#include <fstream>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <vector>
#include "D3Dcompiler.h"
#include "SoundClass.h"
#include "D3dClass.h"


class KFResourceManager{
private:

	struct KFShadresBunch //  базовая связка шейдеров если нет в этом комлексе определенного шейдера то Nullptr
	{
		ID3D11VertexShader *         VS = nullptr; // вертексный шейдер
		ID3D11PixelShader  *         PS = nullptr; // пиксельный шейдер
		ID3D11GeometryShader *       GS = nullptr; // геометрическии шейдер
		ID3D11HullShader *           HS = nullptr; // халл шейдер
//		ID3D11ComputeShader *		 CS = nullptr; // компьют шейдер

	};

public:
	KFResourceManager();
	~KFResourceManager();

	struct ShadersForRenderDecs
	{
		int VertexShaderIndex;
		int PixelShaderIndex;

	};

	std::vector<KFShadresBunch> BunchArr; // массив связок шейдеров

	std::vector<ID3D11VertexShader*>       VertShaderArr; // массив где все вертексные шейдера
	std::vector<ID3D11PixelShader*>        PixShaderArr; // массив где все пиксельные шейдера
	std::vector<ID3D11GeometryShader*>     GeomShaderArr; // массив где все геометрические шейдера
	std::vector<ID3D11HullShader*>         HullShderArr; // массив где все халл шейдера

	std::vector<ID3D11ComputeShader*>      ComeputeShaderArr; // массив где храняться все вычислительные шейдра

	std::vector<int>                       VertexShadreBlobIndexes; // массив интов где для кождого вертексного шейдера находиться индекс его блоба (это так же индекс его инициализании в общих шейдерах)

	void Init(D3DGlobalContext * g_D3DGC , HWND g_hwnd);
	HRESULT InitTextures( WCHAR * kafFilename);
	int InitOneTexture( WCHAR * TextureFilename);
	std::vector<ID3D11ShaderResourceView*> ShaderResourceArr;
	std::vector<ID3D11Resource*> ResourceArr;

	HRESULT InitShaders( WCHAR * kafFilename);
	std::vector<ID3D10Blob*> BlobsArr;
	void SetActiveShadersInProgramm(int ShadersIndex);
	void SetActiveComeputeShader(int ShaderIndex);

	ID3D11ComputeShader* GetComputeShader( int ShaderIndex );


	HRESULT InitSounds(WCHAR * kafFilename,  SoundClass * ActiveSound);

	void ShutDown();

	int InitOneShader( WCHAR * CSOFileName);
	std::vector<ShadersForRenderDecs> MyData;

	void SetNullAllShaders();

private:
	HRESULT hr;
	const WCHAR *Error = L"KFResourceManager Error";
	int TexturesNum , ShadersNum;
	void decodeFile(unsigned char ** DestBuff, WCHAR * kafFilename , HWND hwnd);
	void CharKey(unsigned char * Check);
	D3DGlobalContext * active_D3DGC;
	HWND active_hwnd;

};

#endif
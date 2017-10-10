#pragma once
#include "stdafx.h"

#ifndef _RESORSEMANEGER_H_
#define _RESORSEMANEGER_H_

#include <fstream>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <vector>
#include "D3Dcompiler.h"
#include "SoundClass.h"
#include "D3dClass.h"
#include "VertexBuffers_def.h"

using namespace std;

class ResourceManager{
private:

	struct KFShadresBunch //  базовая связка шейдеров если нет в этом комлексе определенного шейдера то Nullptr
	{
		ID3D11VertexShader *         VS = nullptr; // вертексный шейдер
		ID3D11HullShader *           HS = nullptr; // халл шейдер
		ID3D11DomainShader *		 DS = nullptr; // домайн шейдер
		ID3D11GeometryShader *       GS = nullptr; // геометрическии шейдер
		ID3D11PixelShader  *         PS = nullptr; // пиксельный шейдер
		wchar_t *			  BunchName = nullptr; // имя связки шейдеров 
		ID3D11InputLayout*		 Layout = nullptr; // Input Assm Stage
//		ID3D11ComputeShader *		 CS = nullptr; // компьют шейдер

	};

	wchar_t * StoreShaderName(wchar_t* Name);	// Сохраняем имя шейдера  ( например : ClusteredSM_ps.cso  отрезается ClusteredSM и сохраняется )  

public:
	ResourceManager();
	~ResourceManager();

	struct ShadersForRenderDecs
	{
		int VertexShaderIndex;
		int PixelShaderIndex;

	};

	int GetShaderIndexByName( wchar_t* Name );	
	
	ID3D10Blob* GetShaderBlobByName ( wchar_t* Name );



	vector<KFShadresBunch> BunchArr; // массив связок шейдеров
	
	vector<wchar_t*> ComeputeShaderNames; // отдельный массив имён ComeputeShader
	
	vector< ID3D11VertexShader* >      VertShaderArr;	// массив где все вертексные шейдера
	vector< ID3D11PixelShader* >       PixShaderArr;	// массив где все пиксельные шейдера
	vector< ID3D11GeometryShader* >    GeomShaderArr;	// массив где все геометрические шейдера
	vector< ID3D11HullShader* >        HullShderArr;	// массив где все халл шейдера
	vector< ID3D11DomainShader* >      DomainShderArr;	// массив где все халл шейдера

	vector< ID3D11InputLayout* >	   Layouts;			// Все Layout для шейдеров в системе

	vector< ID3D11ComputeShader* >     ComeputeShaderArr;// массив где храняться все вычислительные шейдра

	vector< ID3D11ShaderResourceView*>ShaderResourceArr;// Массив текстур
	vector< ID3D11Resource* > ResourceArr;				// Массив текстур

	vector< ID3D10Blob* > BlobsArr;

	vector<int>                       VertexShadreBlobIndexes; // массив интов где для кождого вертексного шейдера находиться индекс его блоба (это так же индекс его инициализании в общих шейдерах)

	void Init(D3DGlobalContext * g_D3DGC , HWND g_hwnd);

	bool CreateLayouts ();	// Создаются все нужные Layout для шейдеров по умолчанию
	void ApplyLayoutsToShaderBunches ();

	HRESULT InitTextures( WCHAR * kafFilename);
	int InitOneTexture( WCHAR * TextureFilename);
	
	HRESULT InitShaders( WCHAR * kafFilename);
		
	void SetActiveShadersInProgramm(int ShadersIndex);
	void SetActiveComeputeShader(int ShaderIndex);

	ID3D11ComputeShader* GetComputeShader( int ShaderIndex );
	ID3D11VertexShader*  GetVertexShader ( int ShaderIndex );

	HRESULT InitSounds(WCHAR * kafFilename,  SoundClass * ActiveSound);

	void ShutDown();

	int InitOneShader( WCHAR * CSOFileName);
	
	vector<ShadersForRenderDecs> MyData;

	void SetNullAllShaders();

private:
	HRESULT hr;
	const WCHAR *Error = L"ResourceManager Error";
	int TexturesNum;
	int ShadersNum; // общее количество шейдеров в файле
	void decodeFile(unsigned char ** DestBuff, WCHAR * kafFilename , HWND hwnd);
	void CharKey(unsigned char * Check);
	D3DGlobalContext * active_D3DGC;
	HWND active_hwnd;

};

#endif
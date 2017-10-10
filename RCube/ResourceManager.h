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

	struct KFShadresBunch //  ������� ������ �������� ���� ��� � ���� �������� ������������� ������� �� Nullptr
	{
		ID3D11VertexShader *         VS = nullptr; // ���������� ������
		ID3D11HullShader *           HS = nullptr; // ���� ������
		ID3D11DomainShader *		 DS = nullptr; // ������ ������
		ID3D11GeometryShader *       GS = nullptr; // �������������� ������
		ID3D11PixelShader  *         PS = nullptr; // ���������� ������
		wchar_t *			  BunchName = nullptr; // ��� ������ �������� 
		ID3D11InputLayout*		 Layout = nullptr; // Input Assm Stage
//		ID3D11ComputeShader *		 CS = nullptr; // ������� ������

	};

	wchar_t * StoreShaderName(wchar_t* Name);	// ��������� ��� �������  ( �������� : ClusteredSM_ps.cso  ���������� ClusteredSM � ����������� )  

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



	vector<KFShadresBunch> BunchArr; // ������ ������ ��������
	
	vector<wchar_t*> ComeputeShaderNames; // ��������� ������ ��� ComeputeShader
	
	vector< ID3D11VertexShader* >      VertShaderArr;	// ������ ��� ��� ���������� �������
	vector< ID3D11PixelShader* >       PixShaderArr;	// ������ ��� ��� ���������� �������
	vector< ID3D11GeometryShader* >    GeomShaderArr;	// ������ ��� ��� �������������� �������
	vector< ID3D11HullShader* >        HullShderArr;	// ������ ��� ��� ���� �������
	vector< ID3D11DomainShader* >      DomainShderArr;	// ������ ��� ��� ���� �������

	vector< ID3D11InputLayout* >	   Layouts;			// ��� Layout ��� �������� � �������

	vector< ID3D11ComputeShader* >     ComeputeShaderArr;// ������ ��� ��������� ��� �������������� ������

	vector< ID3D11ShaderResourceView*>ShaderResourceArr;// ������ �������
	vector< ID3D11Resource* > ResourceArr;				// ������ �������

	vector< ID3D10Blob* > BlobsArr;

	vector<int>                       VertexShadreBlobIndexes; // ������ ����� ��� ��� ������� ����������� ������� ���������� ������ ��� ����� (��� ��� �� ������ ��� ������������� � ����� ��������)

	void Init(D3DGlobalContext * g_D3DGC , HWND g_hwnd);

	bool CreateLayouts ();	// ��������� ��� ������ Layout ��� �������� �� ���������
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
	int ShadersNum; // ����� ���������� �������� � �����
	void decodeFile(unsigned char ** DestBuff, WCHAR * kafFilename , HWND hwnd);
	void CharKey(unsigned char * Check);
	D3DGlobalContext * active_D3DGC;
	HWND active_hwnd;

};

#endif
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

	struct KFShadresBunch //  ������� ������ �������� ���� ��� � ���� �������� ������������� ������� �� Nullptr
	{
		ID3D11VertexShader *         VS = nullptr; // ���������� ������
		ID3D11PixelShader  *         PS = nullptr; // ���������� ������
		ID3D11GeometryShader *       GS = nullptr; // �������������� ������
		ID3D11HullShader *           HS = nullptr; // ���� ������
//		ID3D11ComputeShader *		 CS = nullptr; // ������� ������

	};

public:
	KFResourceManager();
	~KFResourceManager();

	struct ShadersForRenderDecs
	{
		int VertexShaderIndex;
		int PixelShaderIndex;

	};

	std::vector<KFShadresBunch> BunchArr; // ������ ������ ��������

	std::vector<ID3D11VertexShader*>       VertShaderArr; // ������ ��� ��� ���������� �������
	std::vector<ID3D11PixelShader*>        PixShaderArr; // ������ ��� ��� ���������� �������
	std::vector<ID3D11GeometryShader*>     GeomShaderArr; // ������ ��� ��� �������������� �������
	std::vector<ID3D11HullShader*>         HullShderArr; // ������ ��� ��� ���� �������

	std::vector<ID3D11ComputeShader*>      ComeputeShaderArr; // ������ ��� ��������� ��� �������������� ������

	std::vector<int>                       VertexShadreBlobIndexes; // ������ ����� ��� ��� ������� ����������� ������� ���������� ������ ��� ����� (��� ��� �� ������ ��� ������������� � ����� ��������)

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
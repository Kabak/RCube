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
#include "Buffers_def.h"
#include "Menu.h"
#include "Texture.h"

#ifdef RCube_DX11
#include "DX11Buffers.h"
#endif 

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
		int						 Layout = NULL;	   // Layout Index ( bind to IAS - Shader from enum LayoutTypes )
		int					VSBlobIndex = NULL;	   // VS Shader Blob index
//		ID3D11ComputeShader *		 CS = nullptr; // ������� ������

	};

	wchar_t * StoreShaderName(wchar_t* Name);	// ��������� ��� �������  ( �������� : ClusteredSM_ps.cso  ���������� ClusteredSM � ����������� )  

public:
	ResourceManager();
	~ResourceManager();

	void Init ( D3DGlobalContext * g_D3DGC );

	int GetShaderIndexByName( wchar_t* Name );				// �������� ����� ������� �� ��� �����
	ID3D11InputLayout* GetLayoutByIndex ( int LayoutNumber);// �������� Layout �� ��� �������
	ID3D10Blob* GetShaderBlobByName ( wchar_t* Name );		// �������� Blob �� ����� �������

// Shaders
	vector< ID3D11VertexShader* >		VertShaderArr;		// ������ ��� ��� ���������� �������
	vector< ID3D11PixelShader* >		PixShaderArr;		// ������ ��� ��� ���������� �������
	vector< ID3D11GeometryShader* >		GeomShaderArr;		// ������ ��� ��� �������������� �������
	vector< ID3D11HullShader* >			HullShderArr;		// ������ ��� ��� ���� �������
	vector< ID3D11DomainShader* >		DomainShderArr;		// ������ ��� ��� ������ �������

	vector< ID3D10Blob* >				BlobsArr;
	vector< KFShadresBunch >			BunchArr;			// ������ ������ ��������

	vector< ID3D11ComputeShader* >		ComeputeShaderArr;	// ������ ��� ��������� ��� �������������� ������
	vector<wchar_t*>					ComeputeShaderNames;// ��������� ������ ��� ComeputeShader

// Layouts
	vector< ID3D11InputLayout* >		Layouts;			// ��� Layout ��� �������� � �������


// Vertex, Instance, Index Buffers
	vector< Flat_Obj_Buffers* >			FlatObjectBuffers;	// Vertex, Instance, Index ������ Flat_Object ( ������ ���� )
	vector< UINT >						UnUsedBuffersIndex;	// ������� �������� FlatObjectBuffers ��������� ��� ���������� �������������

// Textures
	vector <Texture*>					TexturesArr;		// ������ ���� �������
	vector <UINT>						UnusedTextures;		// ������ ��������� �������� �������

	vector< Menu* >						Menus;				// ���� ��������� � ������

// + Textures Works
	bool DeleteTexture ( int Index );

	int CreateTexture ( TextureData _Data ); // �������� ��������

	int CloneTexture ( int Index );	// �������� ������ ��������

	int SaveTextureToFile ( int Index ); // ��������� �������� � ����

	bool RenderTextureOnTexture ( int Index1, int Index2 ); // ���������� �������� 1 �� ��������� 2

// - Textures Works


	bool CreateLayouts ();	// ��������� ��� ������ Layout ��� �������� �� ���������

	int Create_Flat_Obj_Buffers (
//		UINT ModelType,			// ��� ������� �� ��������� � RCube ��� ��������� �� LayoutTypes �������
		bool CPUAccess,			// ������ ���������� ������  
		UINT InstanceAmount,	// ������� Instance � ������
		UINT IndexAmount,		// ���� �� ��������� ������ ��� ���� ������ ( 0 - ��� ��� ���������� �������� � IndexBuffer - IB )
		DXTextureSRV* Texture	// �������� ��������
	);	// �������� ������� ��� �������� �������

	Flat_Obj_Buffers* Get_Flat_ObjectBuffers_ByIndex ( int Index );

	bool Delete_Flat_ObjectBuffers ( int Index );	// �������� ������� ������� �� �������

	void ApplyLayoutsToShaderBunches (); // ��������� ������ Layouts � ��������������� ��������

	bool AddMenu ( Menu* NewMenu ); // �������� ��������� ���� � ������ ���������, ��� ����������� ��������� ����� Render 



	HRESULT InitTextures( WCHAR * kafFilename);		// �������� ������� �� KAF �����
	int InitOneTexture( WCHAR * TextureFilename);	// �������� ����� �������� �� �����
	
	HRESULT InitShaders( WCHAR * kafFilename);		// �������� �������� �� KAF �����
		
	void SetActiveShadersInProgramm(int ShadersIndex);// ��������� �������� ��� ����������� ��������� ��������
	void SetActiveComeputeShader(int ShaderIndex);	// ��������� �������� ��� ��������

	ID3D11ComputeShader* GetComputeShader( int ShaderIndex );
	ID3D11VertexShader*  GetVertexShader ( int ShaderIndex );

	HRESULT InitSounds(WCHAR * kafFilename,  SoundClass * ActiveSound);

	void ShutDown();

	int InitOneShader( WCHAR * CSOFileName);
	
	void SetNullAllShaders();

private:
	HRESULT hr;
	const WCHAR *Error = L"ResourceManager Error";
	int TexturesNum;
	int ShadersNum; // ����� ���������� �������� � �����
	void decodeFile(unsigned char ** DestBuff, WCHAR * kafFilename , HWND hwnd);
	void CharKey(unsigned char * Check);
	
	D3DGlobalContext * Local_D3DGC;

	HWND active_hwnd;

};

#endif
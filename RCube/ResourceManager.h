#pragma once
#include "stdafx.h"

#ifndef _RESORSEMANEGER_H_
#define _RESORSEMANEGER_H_

#include <fstream>
#include <DirectXTex.h>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <vector>
#include "D3Dcompiler.h"
#include "SoundClass.h"
#include "D3dClass.h"
#include "Buffers_def.h"
#include "Menu.h"
#include "Texture.h"
#include "FontClass.h"
#include "3DModel.h"
#include "FrustumClass.h"
//#include "KFModel.h"

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
		WCHAR *				  BunchName = nullptr; // ��� ������ �������� 
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
	vector< Flat_Obj_Buffers* >			FlatObjectBuffers;	// Vertex, Instance, Index ������ Flat_Object ( ������ ���� � BillBoards )
	vector< UINT >						UnusedFlatObjBuffersIndex;	// ������� �������� FlatObjectBuffers ��������� ��� ���������� �������������

// Textures
	vector <Texture*>					TexturesArr;		// ������ ���� �������
	vector <UINT>						UnusedTexturesIndex;// ������ ��������� �������� �������

// Fonts
// ������ ��������� �������
	vector < FontClass* >				RCube_Font;			// ������ ��������� �������

// Sentances
// ������ ��������� ����� ��� ���������
	vector < SentenceType* >			RCube_Sentences;	// ������ ��������� ��������� �����������
	vector <UINT>						UnusedSentenceIndex;// ������ ��������� �������� �����������
	int									TextShaderIndex;	// ����� ������� ��� ������ � ������ �������� 

// 3D Models
// ������ 3D �������� �� �����
	vector <_3DModel* >					_3DModels;			// ������ 3D ������� �� �����
	vector < _3D_Obj_Buffers* >			_3DModelsMeshBuffers;// ������ ���������� ������� 3D �������
	vector <UINT>						Unused3DModelsIndex;// 
	vector <UINT>						Unused3DModelsMeshBuffersIndex;// 

	vector< Menu* >						Menus;				// ���� ��������� � ������

// + 3D Models Works
private:
	bool LoadKFObject ( std::wstring FileName, _3DModel* New_3D_Model ); // ������� �� ��� ����� �����
public:	
	int Add_3D_Object ( wstring FileNameKFO, int InstCout );
	_3DModel* Get3D_Object_Pointer ( int ModelIndex );	// �������� ��������� �� ������ 3D ������ �� ������ �� �������
	_3D_Obj_Buffers* ResourceManager::Get_Mesh_Buffers_By_Index ( int ObjectIndex );// �������� ������ �����
	int Delete_3D_Object ( int ObjectIndex );
	int Delete_3D_Object_Meshes_Buffers ( int ObjectIndex );
	int GetNew3D_Obj_Index ( _3DModel* NewModel );	// �������� ��������� ������ ������ � ������� 3D �������
	int GetNew3D_Obj_Mesh_Buffer_Index ( _3D_Obj_Buffers* NewModel );
	// �������� ������� ��� ������� ���� ������
	int Create_3D_Obj_Mesh_Buffers (
//		UINT ModelType,			// ��� ������� �� ��������� � RCube ��� ��������� �� LayoutTypes �������
		bool CPUAccess,			// ������ ���������� ������  
		UINT InstanceAmount,	// ������� Instance � ������
		UINT IndexAmount		// ���� �� ��������� ������ ��� ���� ������ ( 0 - ��� ��� ���������� �������� � IndexBuffer - IB )
	);	// �������� ������� ��� �������� �������

	void Frustum_3D_Objects ( FrustumClass* Frustum );	// ��������� ��� ��������� �� ����� ������� �������

	// ���������� ������ ��������� � ��������� Instance �������
	// ���� CountingFromInstanceIndex = -1 - ����������� Instance ������� ������� � ��������, ����� ����������� � Amount; 
	// ���� CountingFromInstanceIndex 0 ��� ������, �� ����������� ���������� Amount ������� � �������� � ������� � CountingFromInstanceIndex
	bool Update_3D_Object_InstancesPosBuffer ( int ObjectIndex, int CountingFromInstanceIndex, int Amount );
	
	void InitRandInstansingPoses ( int ObjectIndex, float MaxX, float MinX, float MaxY, float MinY, float MaxZ, float MinZ );
	void InitRandInstansingRots ( int ObjectIndex );
	// ���������� ������� ��������
	void Set_3D_Object_Positon ( int IndexOfObject, int InstanceIndex, float PosX, float PosY, float PosZ );
	void Set_3D_Object_Positon ( int IndexOfObject, int InstanceIndex, XMFLOAT3 &Pos );
	void Set_3D_Object_Rotation ( int IndexOfObject, int InstanceIndex, XMFLOAT4 &Rotation );
	
//	vector <RCudeObjDesc*>ObjDescs;	// ������ 3D �������� �� �����
//	KFModel ** RCubeModelList; // � ���� ������� ��� �������

	Material* GetMaterialData ( int ObjIndex );
	PositionType ** ObjPoses;// ��� ������ ������� ������ ������ �� ���������� �� ������� � �������� ��� �������� ����������� ������� ��� �����������
// !!!!!!!!!!!!!!
	vector<bool> IsObjUseClustering; //������ ����������� ���������� �� ��� ������ ���������� true ���������� false �� ���������� ������ ���������� [������ ���� ������]
// !!!!!!!!!!!!!!
//	int ModelCout; // ���������� �������
	int * CoutObjetcs;
	vector<int> ActiveShaderIndexes;// ������ ������� ������ ��� ������� �������� ��������
// !!!!!!!!!!!	
	int ShaderDrawObjArrSize;
// !!!!!!!!!!!

// - 3D Models Works



// + Sentence Works
// ��������� ����������� � ���������� ��� ����� � ������ ��� -1 ���� �� ������� �������� �����������
	int AddSentence ( SENTENCE_INIT_DATA* data, char* String );

	// �������� ���������� �����������
	int DeleteSentence ( int SentenceNumber );

	// �������� ��� �����������
	void DeleteAllSentences ();

	// ��������� ����� � ������� ����������� �� ������
	void UpdateSentence ( int SentenceNumber, char* text, int positionX, int positionY, float RenderFontSize = 36.0f );
	
	// �������� ������������ ������ �����������
	int GetSentenceMaxLength ( int SentenceNumber );
	
	// �������� ������ ������ � ����������� � ��������
	UINT GetSentenceHeight ( int SentenceNumber );

	// �������� ������ �������� ������ � ��������
	int GetFontHeightInPixels ( int FontNumber );

	int GetPosX ( int SentenceNumber );
	int GetPosY ( int SentenceNumber );
	// �������� ��������� �� ������
	char* GetSentenceText ( int SentenceNumber );
// - Sentence Works

// + Textures Works
	int GetNewTextureIndex ( Texture* NewTexture );	// �������� ��������� ������ �������� � �������

	bool DeleteTexture ( int Index );

	int CreateTexture ( TextureData& _Data ); // �������� ��������

	// ���������� ������ ��������� ��������,
	// ��� -2, ���� Source ������� ��� STAGING, 
	// ��� -1, ���� ����������� �������� �� ����������
	int CloneTextureStaging ( int Source );	// �������� ������ ��������
	
	bool SaveTextureToFile ( int Index, WCHAR* Name ); // ��������� �������� � ����

	bool RenderTextureOnTexture ( int Index1, int Index2 ); // ���������� �������� 1 �� ��������� 2

// - Textures Works


	bool CreateLayouts ();	// ��������� ��� ������ Layout ��� �������� �� ���������

	int Create_Flat_Obj_Buffers (
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
// + Sentence Works	
	bool InitializeSentence ( SentenceType*, int& maxLength );
	void BuildSentanceVertexArray ( FontClass* Font, void* vertices, char* sentence, float& drawX, float& drawY, float& RenderFontSize );
	int GetNewSentenceIndex ( SentenceType* NewSentence );	// �������� ��������� ������ ����������� � �������
// - Sentence Works

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
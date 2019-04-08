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
#include "Terrain_def.h"
#include "Terrain.h"
#include "CubeMap.h"
#include "TimerClass.h"
#include "ParticleSystem.h"
#include "PhysXControler.h"


#ifdef RCube_DX11
#include "DX11Buffers.h"
#endif 

using namespace std;

class ResourceManager{

private:

	template <class BufferType, class UnusedBuffer, class VarType >
	int GetNewBufferIndex ( BufferType& Buffer, UnusedBuffer& IndBuf, VarType* Obj )
	{
		int Index = -1;
		if ( !IndBuf.empty () )
		{
			Index = IndBuf.back ();
			IndBuf.pop_back ();
			Buffer[Index] = Obj;
		}
		else
		{
			Buffer.push_back ( Obj );
			Index = ( int ) ( Buffer.size () - 1 );
		}

		Obj->ThisBufferIndex = Index;

		return Index;
	}

	struct KFShadersBunch //  ������� ������ �������� ���� ��� � ���� �������� ������������� ������� �� Nullptr
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

	struct BuildSentanceData
	{
		FontClass* Font;
		Vertex_FlatObject* vertexes;
		char* text;
		float drawX;
		float drawY;
		float RenderFontSize;
		XMFLOAT4 Colour;

	};

	wchar_t * StoreShaderName(wchar_t* Name);	// ��������� ��� �������  ( �������� : ClusteredSM_ps.cso  ���������� ClusteredSM � ����������� )  

public:
	ResourceManager();
	~ResourceManager();

	void Init ( D3DGlobalContext * g_D3DGC, TimerClass* _Timer, PhysXControler* PhysX );

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
	vector< KFShadersBunch >			BunchArr;			// ������ ������ ��������

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
	vector <UINT>						UnusedFontIndex;	// Free slot

// Sentances
// ������ ��������� ����� ��� ���������
	vector < SentenceType* >			RCube_Sentences;	// ������ ��������� ��������� �����������
	vector <UINT>						UnusedSentenceIndex;// ������ ��������� �������� �����������
	int									TextShaderIndex;	// ����� ������� ��� ������ � ������ �������� 
	int									Temp_Font_StyleShaderIndex;// ����� ������� ��� Bit_map ����� 

// 3D Models
// ������ 3D �������� �� �����
	vector < _3DModel* >				_3DModels;			// ������ 3D ������� �� �����
	vector < _3D_Obj_Buffers* >			_3DModelsMeshBuffers;// ������ ���������� ������� 3D �������
	vector <UINT>						Unused3DModelsIndex;// 
	vector <UINT>						Unused3DModelsMeshBuffersIndex;// 

// LandScape / Terrain
// ������ Terrains �� �����
	vector < Terrain* >					Terrains;			// ������ ���� ���������� / �������
	vector <_3D_Obj_Buffers* >			TerrainVertexBuffers;// ������ ��������� Terrain 
	vector <UINT>						UnusedTerrainsIndex;//
	vector <UINT>						UnusedTerrainsBuffersIndex;// 

// Menu
	vector< Menu* >						Menus;				// ���� ��������� � ������

// Particle Systems
	vector < ParticleSystem* >			ParticleSystems;
	vector < BB_Particles_Buffers* >	BB_ParticleSystems_Buffers;
	vector <UINT>						UnusedParticleSystemIndex;
	vector <UINT>						Unused_BB_BuffersIndex;


// CubeMap
// UnusedFlatObjBuffersIndex
	vector < CubeMap* >					CubeMaps;			// ������ CubeMaps
	vector <UINT>						UnusedCubeMapsIndex;//

// + CubeMap
	int Add_CubeMap ( WCHAR* TextureName );

// - CubeMap

private:

template <class ObjectType, class ObjectIndex, class NewObject >
int GetObjectBuffer ( ObjectType &Object, ObjectIndex &ObjIndex, NewObject &NewObj)
{
	int Index = -1;

	if ( !ObjIndex.empty () )
	{
		Index = ObjIndex.back ();
		ObjIndex.pop_back ();
		Object[Index] = NewObj;
	}
	else
	{
		Object.push_back ( NewObj );
		Index = ( int ) ( Object.size () - 1 );
	}

	return  Index;

}

// + 3D Models Works
	bool LoadKFObject ( std::wstring FileName, _3DModel* New_3D_Model ); // ������� �� ��� ����� �����
	int GetNew3D_Obj_Index ( _3DModel* NewModel );	// �������� ��������� ������ ������ � ������� 3D �������
	int GetNew3D_Obj_Mesh_Buffer_Index ( _3D_Obj_Buffers* NewModel );
public:	
	int Add_3D_Object ( wstring FileNameKFO, int InstCout );
	_3DModel* Get3D_Object_Pointer ( int ModelIndex );	// �������� ��������� �� ������ 3D ������ �� ������ �� �������
	_3D_Obj_Buffers* Get_Mesh_Buffers_By_Index ( int ObjectIndex );// �������� ������ �����
	int Delete_3D_Object ( int ObjectIndex );
	int Delete_3D_Object_Meshes_Buffers ( int ObjectIndex );

	int Create_3D_Obj_Mesh_Buffers (
//		UINT ModelType,			// ��� ������� �� ��������� � RCube ��� ��������� �� LayoutTypes �������
		 int CPUAccess,			// ������ ���������� ������  
		UINT InstanceAmount,	// ������� Instance � ������
		UINT IndexAmount		// ���� �� ��������� ������ ��� ���� ������ ( 0 - ��� ��� ���������� �������� � IndexBuffer - IB )
	);	// �������� ������� ��� ������� ���� ������

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
	
	Material* GetMaterialData ( int ObjIndex );

	vector<int> ActiveShaderIndexes;// ������ ������� ������ ��� ������� �������� ��������

// - 3D Models Works

// + Particle Systems

	int Create_Emitter_BB_Buffers ( int CPUAccess, UINT InstanceAmount, UINT TempTextureIndex, Emitter* NewEmitter );
	int GetNewEmitter_BB_Buffers_Index ( BB_Particles_Buffers* NewBuffer );
	
   bool Delete_Emitter_BB_Buffer ( int BufferIndex );

	BB_Particles_Buffers* GetEmitters_BB_Buffers_By_Index ( int BufferIndex );

	void CreateInitial_BB_VertexBuffer ( Vertex_FlatObject* Vertices, Emitter* NewEmitter );

	int AddParticleSystem (ParticleSystem* ParticleSys);
	bool DeleteParticleSystem ( int ObjectIndex );

// - Particle Systems

// + Terrain
	// If Terrain_GenInit = nullptr , no terrain random generation occure
	int AddTerrain ( int _TerrainTextureIndex, TerrainInitData* NewTerrainData, Terrain_GenInit* TerrainData);
	
	int Get_New_Terrain_Index ( Terrain* TerrainObj );// �������� ��������� ������ Terrain � ������� Terrain
	int Get_Terrain_VB_Index ( _3D_Obj_Buffers* NewBuffer );

	int Delete_Terrain ( int ObjectIndex );
	int Delete_Terrain_Buffers ( int ObjectIndex );
	int Delete_Terrain_Clusters ( Terrain* TerrainObj );	// �������� ��������� Terrain

	int Create_Terrain_Mesh_Buffer ( Terrain* TerrainObj, bool StaticBuffers );	// �������� ������ ��������� Terrain

	int Create_Terrain_Clusters ( Terrain* TerrainObj, bool StaticBuffers );// ������ ����������� ������ �� �������� � ������������� IB ��� ������� ��������

	void Update_Terrain_Clusters_AABB (Terrain* TerrainObj);	// Needed after GenerateLandScape()

	void Calc_Next_Cluster_Start_Pos (int& Start_X_ROW, int& Start_Z_COL, Terrain* TerrainObj );	// ���������� ��������� ������� ���������� ��������

	Terrain* Get_Terrain_Object_Pointer ( int ObjectIndex );	// �������� ��������� �� ������ 3D ������ �� ������ �� �������

	_3D_Obj_Buffers* Get_Terrain_Buffers_By_Index ( int ObjectIndex ); // �������� VB, IB ��� Terrain

	bool Update_Terrain_Position ( int ObjectIndex, PositionType* PositionRotation ); // Caution !  Should be changed in PhysX ( TDO )

	void Calculate_FirstVertexPos ( Terrain* TerrainObj );	// ������ 1-�� �������� � ����������� �� ���������� Terrain
	void GenerateVertexes ( Terrain* TerrainObj );		// ������ ��� �������� �� 1-��
	void GenerateIndexes ( Terrain* TerrainObj, int GridScale );// ������ IB ���� ���������.

	void LandParamChecker (Terrain_GenInit* LandParam);

private :
	void GenerateLandScape ( Terrain* TerrainObj, Terrain_GenInit* InintData);

public :
	//  X_Z - XZ - Position of center ;  HRMinMax : X - Height Min; Y - Height Max ; Z - Radius Min ; W - Radius Max
	bool CreateLandScapeBending (Terrain* TerrainObj, XMFLOAT2 X_Z, XMFLOAT4 HRMinMax);

	bool ReadTerrainFromTexture ( Terrain* TerrainObj, char* FileName );

	void CalculateNormals ( Terrain* TerrainObj );
	
	int	 GenerateRandIntRegion ( int& Max, int& Min );
  float  GenerateRandFloatRegion (float& Max, float& Min);
   void  GenerateRandPoint (Terrain* TerrainObj, PointCoord& Result);	// ��������� ������������ �����

    int  GetIndexByCoord (Terrain* TerrainObj, PointCoord& Coord);		// �������� ������ �������� �� ����������� XZ
   void  GetCoordsByIndex (Terrain* TerrainObj, int Coord, PointCoord& Result);

   void FrustumTerrain ( FrustumClass* Frustum, Terrain* TerrainObj );

   void CalkTerrainClusterAABB ( TerrainCluster* ClasterObject );	// ������ AABB ��������

   void CalkAmountPointsInDir (Terrain* TerrainObj, DirectionsAmount& PointData);// ������ ���������� ����� �� ��������� �� ���� ������������

  float Formula (float& Value);

   void MoveTerrain (Terrain* TerrainObj, DirectionsAmount& PointData, float Height);// �������� Terrain ������ �����

// - Terrain

// + Sentence Works
// ��������� ����������� � ���������� ��� ����� � ������ ��� -1 ���� �� ������� �������� �����������
	int AddSentence ( SENTENCE_INIT_DATA* data, char* String );

	// �������� ���������� �����������
	int DeleteSentence ( int SentenceNumber );

	// �������� ��� �����������
	void DeleteAllSentences ();

	// ��������� ����� � ������� ����������� �� ������
	void UpdateSentence ( int SentenceNumber, char* text, int positionX, int positionY );// , float RenderFontSize = 36.0f );
	
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

	int AddFont ( FontClass* RCubeFont );
	int DeleteFont ( int FontIndex );	// Delete font by index
	// Delete last font in the Engine fonts list
	void DeleteLastFont ();	


// - Sentence Works

// + Textures Works
	int GetNewTextureIndex ( Texture* NewTexture );	// �������� ��������� ������ �������� � �������

	bool DeleteTexture ( int Index );

	int CreateTexture ( TextureData& _Data ); // �������� ��������

	// ���������� ������ ��������� ��������,
	// ��� -2, ���� Source ������� ��� STAGING, 
	// ��� -1, ���� ����������� �������� �� ����������
	int CloneTextureStaging ( int Source );	// �������� ������ ��������

	COLORREF GetScreenPixelColor ( POINT Position );
	
	bool SaveTextureToFile ( int Index, WCHAR* Name ); // ��������� �������� � ����

	bool RenderTextureOnTexture ( int Index1, int Index2 ); // ���������� �������� 1 �� ��������� 2

	// �������� ������ � ������ ��������
	// ���� �������� ��� ������ : ������, ������ ������, � TempData z,w = 0.0f , �� ���� ������� ��������
	void GetTextureParam ( ID3D11ShaderResourceView * Texture, XMFLOAT4& Dimention );
// - Textures Works


	bool CreateLayouts ();	// ��������� ��� ������ Layout ��� �������� �� ���������

	int Create_Flat_Obj_Buffers (
		 int CPUAccess,			// ������ ���������� ������  
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

	// Holding Last shader for checking at render. So we will not set same shader multiple times
	int ActiveShaderNumber;

private:
// + Sentence Works	
	bool InitializeSentence ( SentenceType*, int& maxLength );
	void BuildSentanceVertexArray ( BuildSentanceData* BuildData );
	void BuildSentanceVertexArray ( FontClass* Font, Vertex_FlatObject* vertices, char* sentence, float& drawX, float& drawY, float& RenderFontSize, XMVECTOR& Colour );
	int GetNewSentenceIndex ( SentenceType* NewSentence );	// �������� ��������� ������ ����������� � �������
// - Sentence Works

	HRESULT hr;
	const WCHAR *Error = L"ResourceManager Error";
	int TexturesNum;
	int ShadersNum; // ����� ���������� �������� � �����
	void decodeFile(unsigned char ** DestBuff, WCHAR * kafFilename , HWND hwnd);
	void CharKey(unsigned char * Check);
	
	D3DGlobalContext * Local_D3DGC;
	TimerClass* Local_Timer;
	PhysXControler* Local_PhysX;

	HWND active_hwnd;

};

#endif
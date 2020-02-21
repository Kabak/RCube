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

	char* ErrorString = "Can't split string";

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

	struct KFShadersBunch //  базовая связка шейдеров если нет в этом комлексе определенного шейдера то Nullptr
	{
		ID3D11VertexShader *         VS = nullptr; // вертексный шейдер
		ID3D11HullShader *           HS = nullptr; // халл шейдер
		ID3D11DomainShader *		 DS = nullptr; // домайн шейдер
		ID3D11GeometryShader *       GS = nullptr; // геометрическии шейдер
		ID3D11PixelShader  *         PS = nullptr; // пиксельный шейдер
		WCHAR *				  BunchName = nullptr; // имя связки шейдеров 
		int						 Layout = NULL;	   // Layout Index ( bind to IAS - Shader from enum LayoutTypes )
		int					VSBlobIndex = NULL;	   // VS Shader Blob index
//		ID3D11ComputeShader *		 CS = nullptr; // компьют шейдер

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

	wchar_t * StoreShaderName(wchar_t* Name);	// Сохраняем имя шейдера  ( например : ClusteredSM_ps.cso  отрезается ClusteredSM и сохраняется )  

public:
	ResourceManager();
	~ResourceManager();

	void Init ( D3DGlobalContext * g_D3DGC, TimerClass* _Timer, PhysXControler* PhysX );

	int GetShaderIndexByName( wchar_t* Name );				// Получить номер шейрера по его имени
	ID3D11InputLayout* GetLayoutByIndex ( int LayoutNumber);// Получить Layout по его индексу
	ID3D10Blob* GetShaderBlobByName ( wchar_t* Name );		// Получить Blob по имени шейдера

// Shaders
	vector< ID3D11VertexShader* >		VertShaderArr;		// массив где все вертексные шейдеры
	vector< ID3D11PixelShader* >		PixShaderArr;		// массив где все пиксельные шейдеры
	vector< ID3D11GeometryShader* >		GeomShaderArr;		// массив где все геометрические шейдеры
	vector< ID3D11HullShader* >			HullShderArr;		// массив где все халл шейдеры
	vector< ID3D11DomainShader* >		DomainShderArr;		// массив где все домайн шейдеры

	vector< ID3D10Blob* >				BlobsArr;
	vector< KFShadersBunch >			BunchArr;			// массив связок шейдеров

	vector< ID3D11ComputeShader* >		ComputeShaderArr;	// массив где храняться все вычислительные шейдры
	vector<wchar_t*>					ComputeShaderNames;// отдельный массив имён ComputeShader

// Layouts
	vector< ID3D11InputLayout* >		Layouts;			// Все Layout для шейдеров в системе


// Vertex, Instance, Index Buffers
	vector< Flat_Obj_Buffers* >			FlatObjectBuffers;	// Vertex, Instance, Index буферы Flat_Object ( кнопки меню и BillBoards )
	vector< UINT >						UnusedFlatObjBuffersIndex;	// Индексы удялённых FlatObjectBuffers доступные для повторного использования

// Textures
	vector <Texture*>					TexturesArr;		// Массив всех текстур
	vector <UINT>						UnusedTexturesIndex;// Массив свободных индексов текстур

// Fonts
// Список доступных шрифтов
	vector < FontClass* >				RCube_Font;			// Список созданных шрифтов
	vector <UINT>						UnusedFontIndex;	// Free slot

// Sentances
// Список текстовых строк для отрисовки
	vector < SentenceType* >			RCube_Sentences;	// Список созданных текстовых предложений
	vector <UINT>						UnusedSentenceIndex;// Массив свободных индексов предложений
	int									TextShaderIndex;	// Номер шейдера для текста в списке шейдеров 
	int									Temp_Font_StyleShaderIndex;// Номер шейлера для Bit_map Мышки 

	vector <char*>						Strings_List;		// Массив строк подлежащих отрисовке

// 3D Models
// Список 3D объектов на сцене
	vector < _3DModel* >				_3DModels;			// Список 3D моделей на сцене
	vector < _3D_Obj_Buffers* >			_3DModelsMeshBuffers;// Список вертексных буферов 3D моделей
	vector <UINT>						Unused3DModelsIndex;// 
	vector <UINT>						Unused3DModelsMeshBuffersIndex;// 

// LandScape / Terrain
// Список Terrains на сцене
	vector < Terrain* >					Terrains;			// Список карт местностей / уровней
	vector <_3D_Obj_Buffers* >			TerrainVertexBuffers;// Массив вертексов Terrain 
	vector <UINT>						UnusedTerrainsIndex;//
	vector <UINT>						UnusedTerrainsBuffersIndex;// 

// Menu
	vector< Menu* >						Menus;				// Меню созданные в движке

// Particle Systems
	vector < ParticleSystem* >			ParticleSystems;
	vector < BB_Particles_Buffers* >	BB_ParticleSystems_Buffers;
	vector <UINT>						UnusedParticleSystemIndex;
	vector <UINT>						Unused_BB_BuffersIndex;


// CubeMap
// UnusedFlatObjBuffersIndex
	vector < CubeMap* >					CubeMaps;			// Список CubeMaps
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
	bool LoadKFObject ( std::wstring FileName, _3DModel* New_3D_Model ); // достает из кфо файла обьет
	int GetNew3D_Obj_Index ( _3DModel* NewModel );	// Получить свободный индекс модели в массиве 3D моделей
	int GetNew3D_Obj_Mesh_Buffer_Index ( _3D_Obj_Buffers* NewModel );
public:	
	int Add_3D_Object ( wstring FileNameKFO, int InstCout );
	_3DModel* Get3D_Object_Pointer ( int ModelIndex );	// Получить указатель на объект 3D модели из списка по индексу
	_3D_Obj_Buffers* Get_Mesh_Buffers_By_Index ( int ObjectIndex );// Получить буферы мешаж
	int Delete_3D_Object ( size_t ObjectIndex );
	int Delete_3D_Object_Meshes_Buffers ( size_t ObjectIndex );

	int Create_3D_Obj_Mesh_Buffers (
//		UINT ModelType,			// Тип объекта из возможных в RCube для рисования по LayoutTypes объекта
		 int CPUAccess,			// способ обновления буферы  
		UINT InstanceAmount,	// Сколько Instance в модели
		UINT IndexAmount		// Есть ли индексный массив для меша модели ( 0 - нет или количество индексов в IndexBuffer - IB )
	);	// Создание буферов для каждого меша модели

	void Frustum_3D_Objects ( FrustumClass* Frustum );	// Отсеиваем все невидимые нв кадре объекты моделей

	// Обновление буферы положений и поворотов Instance объекта
	// Если CountingFromInstanceIndex = -1 - обновляются Instance объекта начиная с нулевого, общим количеством в Amount; 
	// Если CountingFromInstanceIndex 0 или больше, то обновляется количество Amount начиная с элемента с номером в CountingFromInstanceIndex
	bool Update_3D_Object_InstancesPosBuffer ( int ObjectIndex, int CountingFromInstanceIndex, int Amount );
	
	void InitRandInstansingPoses ( int ObjectIndex, float MaxX, float MinX, float MaxY, float MinY, float MaxZ, float MinZ );
	void InitRandInstansingRots ( int ObjectIndex );
	// Обновление позиции объектам
	void Set_3D_Object_Positon ( int IndexOfObject, int InstanceIndex, float PosX, float PosY, float PosZ );
	void Set_3D_Object_Positon ( int IndexOfObject, int InstanceIndex, XMFLOAT3 &Pos );
	void Set_3D_Object_Rotation ( int IndexOfObject, int InstanceIndex, XMFLOAT4 &Rotation );
	
	Material* GetMaterialData ( int ObjIndex );

	vector<int> ActiveShaderIndexes;// массив котопый хранит все индексы активных шейдеров

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
	
	int Get_New_Terrain_Index ( Terrain* TerrainObj );// Получить свободный индекс Terrain в массиве Terrain
	int Get_Terrain_VB_Index ( _3D_Obj_Buffers* NewBuffer );

	int Delete_Terrain ( size_t ObjectIndex );
	int Delete_Terrain_Buffers ( size_t ObjectIndex );
	int Delete_Terrain_Clusters ( Terrain* TerrainObj );	// Удаление кластеров Terrain

	int Create_Terrain_Mesh_Buffer ( Terrain* TerrainObj, bool StaticBuffers );	// Создание буфера вертексов Terrain

	int Create_Terrain_Clusters ( Terrain* TerrainObj, bool StaticBuffers );// Раздел вертексного буфера на кластеры и инициализация IB для каждого кластера

	void Update_Terrain_Clusters_AABB (Terrain* TerrainObj);	// Needed after GenerateLandScape()

	void Calc_Next_Cluster_Start_Pos (int& Start_X_ROW, int& Start_Z_COL, Terrain* TerrainObj );	// Определить стартовую позицию следуещего кластера

	Terrain* Get_Terrain_Object_Pointer ( int ObjectIndex );	// Получить указатель на объект 3D модели из списка по индексу

	_3D_Obj_Buffers* Get_Terrain_Buffers_By_Index ( int ObjectIndex ); // Получаем VB, IB для Terrain

	bool Update_Terrain_Position ( int ObjectIndex, PositionType* PositionRotation ); // Caution !  Should be changed in PhysX ( TDO )

	void Calculate_FirstVertexPos ( Terrain* TerrainObj );	// Расчёт 1-го вертекса в зависимости от параметров Terrain
	void GenerateVertexes ( Terrain* TerrainObj );		// Создаёт все вертексы по 1-му
	void GenerateIndexes ( Terrain* TerrainObj, int GridScale );// Создаёт IB всех вертексов.

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
   void  GenerateRandPoint (Terrain* TerrainObj, PointCoord& Result);	// Генерация произвольной точки

    int  GetIndexByCoord (Terrain* TerrainObj, PointCoord& Coord);		// Получить индекс вертекса по координатам XZ
   void  GetCoordsByIndex (Terrain* TerrainObj, int Coord, PointCoord& Result);

   void FrustumTerrain ( FrustumClass* Frustum, Terrain* TerrainObj );

   void CalkTerrainClusterAABB ( TerrainCluster* ClasterObject );	// Расчёт AABB кластера

   void CalkAmountPointsInDir (Terrain* TerrainObj, DirectionsAmount& PointData);// Расчёт количества точек от эпицентра во всех направлениях

  float Formula (float& Value);

   void MoveTerrain (Terrain* TerrainObj, DirectionsAmount& PointData, float Height);// Изменить Terrain вокруг точки

// - Terrain

// + Sentence Works
// Добавляет предложение и возвращает его номер в списке или -1 если не удалось добавить предложение
	int AddSentence ( SENTENCE_INIT_DATA* data, char* String );

	// Вытирает конкретное предложение
	int DeleteSentence ( int SentenceNumber );

	// Вытирает все предложения
	void DeleteAllSentences ();

	void DeleteGroupSentences ( int GroupOrLevelIndex ); // Delete Level / Group of Sentences

	void Split_Sentence ( AboutMenuElement *AboutElement, int SymbolsInString, vector <char*> *Strings_List, bool Speed_beauty );	// Make strings from one string
	void Split_Sentence ( AboutMenuElement *AboutElement, vector <char*> *Strings_List, bool Speed_beauty );	// Make strings from one string
	 
	void SplitString ( char* String, vector< char* > *StringsList, int Size );

	int CreateSentencesGroup ( AboutMenuElement *AboutElement, vector <char*> *Strings_List ); // Groupe Sentences  ( for example : About messages )   Return Group Index
	
	int CheckStringInBounds ( char *TempString,int &MaxCharAmount, int &SymbolWidth,int &MaxLengthInPixels ); // Check string fitting by width
	int CheckMaxSymbolWord ( char* String );

	// Обновляет текст и позицию предложения на экране
	void UpdateSentence ( int SentenceNumber, char* text, int positionX, int positionY );// , float RenderFontSize = 36.0f );
	void UpdateSentence (int SentenceNumber, char* text);
	// Получить максимальную длинну предложения
	int GetSentenceMaxLength ( int SentenceNumber );
	
	// Получить высоту текста в предложения в пикселях
	UINT GetSentenceHeight ( int SentenceNumber );

	// Получить высоту текстуры шрифта в пикселях
	int GetFontHeightInPixels ( int FontNumber );
	int GetFontWidthInPixels ( int FontNumber );

	int GetSentencePosX ( int SentenceNumber );
	int GetSentencePosY ( int SentenceNumber );

	void GetSentencePos ( int SentenceNumber, RCube_XY_POS* XY_Position );

	// Получить указатель на строку
	char* GetSentenceText ( int SentenceNumber );

	int AddFont ( FontClass* RCubeFont );
	int DeleteFont ( int FontIndex );	// Delete font by index
	// Delete last font in the Engine fonts list
	void DeleteLastFont ();	


// - Sentence Works

// + Textures Works
	int GetNewTextureIndex ( Texture* NewTexture );	// Получить свободный индекс текстуры в системе

	bool DeleteTexture ( int Index );

	int CreateTexture ( TextureData& _Data ); // Создание текстуры

	int Create2DTexture ( UINT Width, UINT Height ); // Create 2D texture + ShaderResource

	int Create2DTexture_From_2DTextureRegion ( int SourceTextureIndex, TextureRegion NewTextureDimension );
	int Create2DTexture_From_2DTextureRegion ( ID3D11ShaderResourceView * Texture, TextureRegion NewTextureDimension );

	int CreateCopy2DTexture ( int CopiedTextureIndex ); // Copy 2D Texture

	// Возвращает индекс созданной текстуры,
	// или -2, если Source создана как STAGING, 
	// или -1, если клонировать текстуру не получилось
	int CloneTextureStaging ( int Source );	// Создание коппии текстуры

	COLORREF GetScreenPixelColor ( POINT Position );
	
	bool SaveTextureToFile ( int Index, WCHAR* Name ); // Сохранить текстуру в файл

	bool RenderTextureOnTexture ( int Index1, int Index2 ); // Нарисовать текстуру 1 на текструре 2

	// Получить высоту и ширину текстуры
	// Если текстура для кнопок : меньше, больше задана, и TempData z,w = 0.0f , то берём размеры текстуры
	void GetTextureParam ( ID3D11ShaderResourceView * Texture, XMFLOAT4& Dimention );
// - Textures Works


	bool CreateLayouts ();	// Создаются все нужные Layout для шейдеров по умолчанию

	int Create_Flat_Obj_Buffers (
		 int CPUAccess,			// способ обновления буферов  
		UINT InstanceAmount,	// Сколько Instance в модели
		UINT IndexAmount,		// Есть ли индексный массив для меша модели ( 0 - нет или количество индексов в IndexBuffer - IB )
		DXTextureSRV* Texture	// Основная текстура
	);	// Создание буферов для плоского объекта

	Flat_Obj_Buffers* Get_Flat_ObjectBuffers_ByIndex ( int Index );

	bool Delete_Flat_ObjectBuffers ( int Index );	// удаление буферов объекта по индексу

	void ApplyLayoutsToShaderBunches (); // Привязать нужные Layouts к соответствующим шейлерам

	bool AddMenu ( Menu* NewMenu ); // Добавить созданное меню в список созданных, для последующей отрисовки через Render 



	HRESULT InitTextures( WCHAR * kafFilename);		// Создание текстур из KAF файла
	int InitOneTexture( WCHAR * TextureFilename);	// Создание одной текстуры из файла
	
	HRESULT InitShaders( WCHAR * kafFilename);		// Создание шейдеров из KAF файла
		
	void SetActiveShadersInProgramm(int ShadersIndex);// Установка шейдеров для последующей отрисовки объектов
	void SetActiveComputeShader(int ShaderIndex);	// Установка шейдеров для расчётов

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
	int GetNewSentenceIndex ( SentenceType* NewSentence );	// Получить свободный индекс предложения в системе
// - Sentence Works

	HRESULT hr;
	const WCHAR *Error = L"ResourceManager Error";
		  WCHAR *Prefix = L"Can't create ";
		  WCHAR *InitOneShader_str = L" in InitOneShader.";
		  WCHAR *InitShader = L" in InitShaders";
		  WCHAR *InitTexture = L" in InitTextures";
		  WCHAR *InitSound = L" in InitSound";
		  WCHAR *CreateLayout = L" in CreateLayouts";
		  WCHAR *CreateTexture_str = L" in CreateTexture";
		  WCHAR *VS_str = L"Vertex Shader ";
		  WCHAR *PS_str = L"Pixel Shader ";
		  WCHAR *GS_str = L"Geometry Shader ";
		  WCHAR *HS_str = L"Hull Shader ";
		  WCHAR *DS_str = L"Domain Shader ";
		  WCHAR *CS_str = L"Compute Shader ";

		  WCHAR *ErrorMessage;
		  void RCUBE_ErrorMessageShow ( WCHAR* Prefix, WCHAR* String, WCHAR* Postfix );

	int TexturesNum;
	int ShadersNum; // общее количество шейдеров в файле
	void decodeFile(unsigned char ** DestBuff, WCHAR * kafFilename , HWND hwnd);
	void CharKey(unsigned char * Check);
	
	D3DGlobalContext * Local_D3DGC;
	TimerClass* Local_Timer;
	PhysXControler* Local_PhysX;

	HWND active_hwnd;

};

#endif
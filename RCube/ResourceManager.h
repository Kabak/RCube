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

#ifdef RCube_DX11
#include "DX11Buffers.h"
#endif 

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
		WCHAR *				  BunchName = nullptr; // имя связки шейдеров 
		int						 Layout = NULL;	   // Layout Index ( bind to IAS - Shader from enum LayoutTypes )
		int					VSBlobIndex = NULL;	   // VS Shader Blob index
//		ID3D11ComputeShader *		 CS = nullptr; // компьют шейдер

	};

	wchar_t * StoreShaderName(wchar_t* Name);	// Сохраняем имя шейдера  ( например : ClusteredSM_ps.cso  отрезается ClusteredSM и сохраняется )  

public:
	ResourceManager();
	~ResourceManager();

	void Init ( D3DGlobalContext * g_D3DGC );

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
	vector< KFShadresBunch >			BunchArr;			// массив связок шейдеров

	vector< ID3D11ComputeShader* >		ComeputeShaderArr;	// массив где храняться все вычислительные шейдры
	vector<wchar_t*>					ComeputeShaderNames;// отдельный массив имён ComeputeShader

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

// Sentances
// Список текстовых строк для отрисовки
	vector < SentenceType* >			RCube_Sentences;	// Список созданных текстовых предложений
	vector <UINT>						UnusedSentenceIndex;// Массив свободных индексов предложений
	int									TextShaderIndex;	// Номер шейдера для текста в списке шейдеров 

// 3D Models
// Список 3D объектов на сцене
	vector < _3DModel* >				_3DModels;			// Список 3D моделей на сцене
	vector < _3D_Obj_Buffers* >			_3DModelsMeshBuffers;// Список вертексных буферов 3D моделей
	vector <UINT>						Unused3DModelsIndex;// 
	vector <UINT>						Unused3DModelsMeshBuffersIndex;// 

// LandScape / Terrain
	vector < Terrain* >					Terrains;			// Список карт местностей / уровней
	vector <_3D_Obj_Buffers* >			TerrainVertexBuffers;// Массив вертексов Terrain 
	vector <UINT>						UnusedTerrainsIndex;//
	vector <UINT>						UnusedTerrainsBuffersIndex;// 

// CubeMap
// UnusedFlatObjBuffersIndex
	vector < CubeMap* >					CubeMaps;			// Список CubeMaps
	vector <UINT>						UnusedCubeMapsIndex;//


	vector< Menu* >						Menus;				// Меню созданные в движке

// + CubeMap
	int Add_CubeMap ( WCHAR* TextureName );

// - CubeMap

// + 3D Models Works
private:
	bool LoadKFObject ( std::wstring FileName, _3DModel* New_3D_Model ); // достает из кфо файла обьет
	int GetNew3D_Obj_Index ( _3DModel* NewModel );	// Получить свободный индекс модели в массиве 3D моделей
	int GetNew3D_Obj_Mesh_Buffer_Index ( _3D_Obj_Buffers* NewModel );
public:	
	int Add_3D_Object ( wstring FileNameKFO, int InstCout );
	_3DModel* Get3D_Object_Pointer ( int ModelIndex );	// Получить указатель на объект 3D модели из списка по индексу
	_3D_Obj_Buffers* Get_Mesh_Buffers_By_Index ( int ObjectIndex );// Получить буферы мешаж
	int Delete_3D_Object ( int ObjectIndex );
	int Delete_3D_Object_Meshes_Buffers ( int ObjectIndex );

	int Create_3D_Obj_Mesh_Buffers (
//		UINT ModelType,			// Тип объекта из возможных в RCube для рисования по LayoutTypes объекта
		bool CPUAccess,			// способ обновления буферы  
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
	
//	vector <RCudeObjDesc*>ObjDescs;	// Список 3D объектов на сцене
//	KFModel ** RCubeModelList; // в этом массиве все обьекты

	Material* GetMaterialData ( int ObjIndex );
	PositionType ** ObjPoses;// это массив который хранит данные об указателях на позиции и поворота для наиболее скоростного доступа без посредников
// !!!!!!!!!!!!!!
	vector<bool> IsObjUseClustering; //массив описывающий использует ли тип модели кластеринг true использует false не использует формат заполнения [индекс типа модели]
// !!!!!!!!!!!!!!
//	int ModelCout; // количество моделей
	int * CoutObjetcs;
	vector<int> ActiveShaderIndexes;// массив котопый хранит все индексы активных шейдеров
// !!!!!!!!!!!	
	int ShaderDrawObjArrSize;
// !!!!!!!!!!!

// - 3D Models Works

// + Terrain
	int AddTerrain ( int _TerrainTextureIndex, TerrainInitData* NewTerrainData );
	
	int Get_New_Terrain_Index ( Terrain* TerrainObj );// Получить свободный индекс Terrain в массиве Terrain
	int Get_Terrain_VB_Index ( _3D_Obj_Buffers* NewBuffer );

	int Delete_Terrain ( int ObjectIndex );
	int Delete_Terrain_Buffers ( int ObjectIndex );
	
	int Create_Terrain_Mesh_Buffer ( Terrain* TerrainObj );	// Создание буфера вертексов Terrain
	int Create_Terrain_Clusters ( Terrain* TerrainObj );// Раздел вертексного буфера на кластеры и инициализация IB для каждого кластера

	Terrain* Get_Terrain_Object_Pointer ( int ObjectIndex );	// Получить указатель на объект 3D модели из списка по индексу
	_3D_Obj_Buffers* Get_Terrain_Buffers_By_Index ( int ObjectIndex ); // Получаем VB, IB для Terrain
	bool Update_Terrain_Position ( int ObjectIndex, PositionType* PositionRotation );

	void Calculate_FirstVertexPos ( Terrain* TerrainObj );	// Расчёт 1-го вертекса в зависимости от параметров Terrain
	void GenerateVertexes ( Terrain* TerrainObj );		// Создаёт все вертексы по 1-му
	void GenerateIndexes ( Terrain* TerrainObj, int GridScale );// Создаёт IB всех вертексов.
//	 int ComeputeIndex ( Terrain* TerrainObj, int X, int Z ); // рассчёт номера индекса по позиции XZ вертекса
	void GetQuadIndex ( Terrain* TerrainObj , int UL, int DR , Point* _Result); // Расчёт индексов вертексов составляющих квадрат | UL - UpLeft , DR - DownRight индекс вертекса


	bool GenerateRandTerrain ( Terrain* TerrainObj, TerrainParam* LandParam, float VertixesIndent );
	bool GenerateRandTerrain ( Terrain* TerrainObj );	// НОВАЯ ВЕРСИЯ
	
	bool ReadTerrainFromTexture ( Terrain* TerrainObj, TerrainParam* LandParam, float VertixesIndent );
	bool ReadTerrainFromTexture ( Terrain* TerrainObj, char* FileName );	// НОВАЯ ВЕРСИЯ

	void CheckClustersVertixes ( Terrain* TerrainObj );
	void CheckDrawLenghtParam ( Terrain* TerrainObj );
	void CalculateNormals ( Terrain* TerrainObj );
	
	void GenerateLandScape ( Terrain* TerrainObj, TerrainParam* LandParam, HeightMapType* m_heightMap );
	void GenerateLandScape ( Terrain* TerrainObj, TerrainInitData* Data);  // НОВАЯ ВЕРСИЯ
	
	int GenerateRandRadius ( int MaxRadius, int MinRadius );
	Point GenerateRandPoint ( Terrain* TerrainObj );
	void CheckDrawLenghtParam ( HWND hwnd );
	void CheckClustersVertixes ( HWND hwnd );
	int ComeputeIndex ( int X, int Z );
	void TerrainFrame ( bool CameraReplaseData, XMVECTOR &CameraPosition );
	void LandParamChecker ( TerrainParam* LandParam );

// - Terrain

// + Sentence Works
// Добавляет предложение и возвращает его номер в списке или -1 если не удалось добавить предложение
	int AddSentence ( SENTENCE_INIT_DATA* data, char* String );

	// Вытирает конкретное предложение
	int DeleteSentence ( int SentenceNumber );

	// Вытирает все предложения
	void DeleteAllSentences ();

	// Обновляет текст и позицию предложения на экране
	void UpdateSentence ( int SentenceNumber, char* text, int positionX, int positionY, float RenderFontSize = 36.0f );
	
	// Получить максимальную длинну предложения
	int GetSentenceMaxLength ( int SentenceNumber );
	
	// Получить высоту текста в предложения в пикселях
	UINT GetSentenceHeight ( int SentenceNumber );

	// Получить высоту текстуры шрифта в пикселях
	int GetFontHeightInPixels ( int FontNumber );

	int GetPosX ( int SentenceNumber );
	int GetPosY ( int SentenceNumber );
	// Получить указатель на строку
	char* GetSentenceText ( int SentenceNumber );
// - Sentence Works

// + Textures Works
	int GetNewTextureIndex ( Texture* NewTexture );	// Получить свободный индекс текстуры в системе

	bool DeleteTexture ( int Index );

	int CreateTexture ( TextureData& _Data ); // Создание текстуры

	// Возвращает индекс созданной текстуры,
	// или -2, если Source создана как STAGING, 
	// или -1, если клонировать текстуру не получилось
	int CloneTextureStaging ( int Source );	// Создание коппии текстуры
	
	bool SaveTextureToFile ( int Index, WCHAR* Name ); // Сохранить текстуру в файл

	bool RenderTextureOnTexture ( int Index1, int Index2 ); // Нарисовать текстуру 1 на текструре 2

// - Textures Works


	bool CreateLayouts ();	// Создаются все нужные Layout для шейдеров по умолчанию

	int Create_Flat_Obj_Buffers (
		bool CPUAccess,			// способ обновления буферы  
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
	void SetActiveComeputeShader(int ShaderIndex);	// Установка шейдеров для расчётов

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
	int GetNewSentenceIndex ( SentenceType* NewSentence );	// Получить свободный индекс предложения в системе
// - Sentence Works

	HRESULT hr;
	const WCHAR *Error = L"ResourceManager Error";
	int TexturesNum;
	int ShadersNum; // общее количество шейдеров в файле
	void decodeFile(unsigned char ** DestBuff, WCHAR * kafFilename , HWND hwnd);
	void CharKey(unsigned char * Check);
	
	D3DGlobalContext * Local_D3DGC;

	HWND active_hwnd;

};

#endif
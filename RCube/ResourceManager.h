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

	struct KFShadresBunch //  базовая связка шейдеров если нет в этом комлексе определенного шейдера то Nullptr
	{
		ID3D11VertexShader *         VS = nullptr; // вертексный шейдер
		ID3D11HullShader *           HS = nullptr; // халл шейдер
		ID3D11DomainShader *		 DS = nullptr; // домайн шейдер
		ID3D11GeometryShader *       GS = nullptr; // геометрическии шейдер
		ID3D11PixelShader  *         PS = nullptr; // пиксельный шейдер
		wchar_t *			  BunchName = nullptr; // имя связки шейдеров 
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
	vector< Flat_Obj_Buffers* >			FlatObjectBuffers;	// Vertex, Instance, Index буферы Flat_Object ( кнопки меню )
	vector< UINT >						UnUsedBuffersIndex;	// Индексы удялённых FlatObjectBuffers доступные для повторного использования

// Textures
	vector <Texture*>					TexturesArr;		// Массив всех текстур
	vector <UINT>						UnusedTextures;		// Массив свободных индексов текстур

	vector< Menu* >						Menus;				// Меню созданные в движке

// + Textures Works
	bool DeleteTexture ( int Index );

	int CreateTexture ( TextureData _Data ); // Создание текстуры

	int CloneTexture ( int Index );	// Создание коппии текстуры

	int SaveTextureToFile ( int Index ); // Сохранить текстуру в файл

	bool RenderTextureOnTexture ( int Index1, int Index2 ); // Нарисовать текстуру 1 на текструре 2

// - Textures Works


	bool CreateLayouts ();	// Создаются все нужные Layout для шейдеров по умолчанию

	int Create_Flat_Obj_Buffers (
//		UINT ModelType,			// Тип объекта из возможных в RCube для рисования по LayoutTypes объекта
		bool CPUAccess,			// способ обновления буфера  
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
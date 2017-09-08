#pragma once
#include "stdafx.h"
///////////////////////////////////////////////////////////////////////////////
// Filename: soundclass.h
///////////////////////////////////////////////////////////////////////////////
#ifndef _SOUNDCLASS_H_
#define _SOUNDCLASS_H_


/////////////
// LINKING //
/////////////
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")


//////////////
// INCLUDES //
//////////////
#include <vector>
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>

using namespace std;

//https://msdn.microsoft.com/en-us/library/windows/desktop/ee416964%28v=vs.85%29.aspx
//http://www.gamedev.ru/community/swrus/articles/ds_usage_basic
//http://www.gamedev.ru/industry/articles/bravada_experience
//http://www.codeproject.com/Articles/11184/The-ultimate-Managed-DirectSound-Tutorial-Part
//http://www.codeproject.com/Articles/11191/The-Ultimate-Managed-DirectSound-Tutorial-Part
// ogg decoder
//http://nothings.org/stb_vorbis/

// 		if ( !m_Sound->_3DSoundPlaying(0) ) 
//		m_Sound->PlayWaveFile3D(0, 0,0,0,40);
//		if ( !m_Sound->StereoSoundPlaying(0) )
//		m_Sound->PlayWaveFile(0);
///////////////////////////////////////////////////////////////////////////////
// Class name: SoundClass
///////////////////////////////////////////////////////////////////////////////
class SoundClass
{
public:
	SoundClass();
	~SoundClass();

	bool Initialize(HWND);
	void Shutdown();

	bool PlayWaveFile(UINT Index);
	bool PlayWaveFile3D(UINT Index, float x, float y, float z, D3DVALUE dopler);
// Проверка играет ли стереозвук
	bool StereoSoundPlaying(UINT Index);
// Проверка играет ли 3D звук
	bool _3DSoundPlaying(UINT Index);
// Проверка играет ли музыка
	bool OggMusicPlaying(UINT Index);
// Громкость глобальная и для конкретного звука
//	bool SetEffectsVolume();
//	bool SetMusicsVolume();
	void SetEffectVolume(UINT Index, long Volume);
	void SetMusicVolume(UINT Index, long Volume);
	void SetEffectsOFF();
	void SetMusicOFF();

	LPDWORD BuffersStatus;
	vector < IDirectSoundBuffer8* > SoundStereo;
	vector < IDirectSoundBuffer8* > _3DSound;
	vector < IDirectSound3DBuffer8* > _3DSoundControl;

// Отридцательное значение -10 000 - минимальный звук = 0 = DSBVOLUME_MIN
	long MusicVolume;
	long EffectsVolume;

	// Максимальный размер буфера для музыкального файла ( декодированного ) - 10 000 000 байт = 10 Мбайт
	bool LoadStereoWaveFile(unsigned char*, IDirectSoundBuffer8**);
	bool LoadMonoWaveFile(unsigned char*, IDirectSoundBuffer8**, IDirectSound3DBuffer8**);
	bool LoadOggMusicFile(unsigned char*, IDirectSoundBuffer8**, int size);

private:
	void ShutdownDirectSound();

	IDirectSound8* m_DirectSound;
	IDirectSoundBuffer* m_primaryBuffer;
	IDirectSound3DListener8* m_listener;
	struct WaveHeaderType
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};

};

#endif
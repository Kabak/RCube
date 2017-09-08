///////////////////////////////////////////////////////////////////////////////
// Filename: soundclass.cpp
///////////////////////////////////////////////////////////////////////////////
// http://nothings.org/stb_vorbis/

#include "stdafx.h"
#include "soundclass.h"
#include "stb_vorbis.cpp"

SoundClass::SoundClass()
{
	m_DirectSound = 0;
  m_primaryBuffer = 0;
	   m_listener = 0;
	BuffersStatus = new DWORD;
   *BuffersStatus = 0;
	  MusicVolume = DSBVOLUME_MAX;//DSBVOLUME_MIN
	EffectsVolume = DSBVOLUME_MAX;//DSBVOLUME_MIN
}


SoundClass::~SoundClass()
{
	delete BuffersStatus;

	UINT i = 0;
	UINT j = (UINT)SoundStereo.size();
/*	do
	{
		SoundStereo[i]->Release();
		++i;
	} while (i < j);*/
	SoundStereo.clear();

	i = 0;
	j = (UINT)_3DSound.size();

	/*do
	{
		_3DSound[i]->Release();
		++i;
	} while (i < j);*/
	_3DSound.clear();

	i = 0;
	j = (UINT)_3DSoundControl.size();

	/*do
	{
		_3DSoundControl[i]->Release();
		++i;
	} while (i < j);*/
	_3DSoundControl.clear();
}


bool SoundClass::Initialize(HWND hwnd)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;


	// Initialize the direct sound interface pointer for the default sound device.
	result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Set the cooperative level to priority so the format of the primary sound buffer can be modified.
	result = m_DirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the primary buffer description.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Get control of the primary sound buffer on the default sound device.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if (FAILED(result))
	{
		return false;
	}
	// The format of the primary buffer stays the same, only secondary buffers are required to be mono. Direct Sound will take care of mixing different 
	// format secondary buffers into the primary buffer automatically.
	// Setup the format of the primary sound bufffer.
	// In this case it is a .WAV file recorded at 44,100 samples per second in 16-bit stereo (cd audio format).
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the primary buffer to be the wave format specified.
	result = m_primaryBuffer->SetFormat(&waveFormat);
	if (FAILED(result))
	{
		return false;
	}

	// Once the primary buffer is created we can obtain a listener interface from it. This will allow us to position 
	// the listener in 3D space in relation to the other 3D positioned sounds. 

	// Obtain a listener interface.
	result = m_primaryBuffer->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)&m_listener);
	if (FAILED(result))
	{
		return false;
	}

	// To start we set the initial position of the listener at the origin of the world. The DS3D_IMMEDIATE parameter tells 
	// DirectSound to set it right away and not to defer it for batch processing later. 

	// Set the initial position of the listener to be in the middle of the scene.
	m_listener->SetPosition(0.0f, 0.0f, 0.0f, DS3D_IMMEDIATE);

	return true;
}


void SoundClass::Shutdown()
{
	// Shutdown the Direct Sound API.
	ShutdownDirectSound();

	return;
}

void SoundClass::ShutdownDirectSound()
{
	// Release the listener interface.
	if(m_listener)
	{
		m_listener->Release();
		m_listener = 0;
	}
	// Release the primary sound buffer pointer.
	if(m_primaryBuffer)
	{
		m_primaryBuffer->Release();
		m_primaryBuffer = 0;
	}

	// Release the direct sound interface pointer.
	if(m_DirectSound)
	{
		m_DirectSound->Release();
		m_DirectSound = 0;
	}

	return;
}


bool SoundClass::LoadMonoWaveFile(unsigned char* file, IDirectSoundBuffer8** secondaryBuffer, IDirectSound3DBuffer8** secondary3DBuffer)
{

	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT result;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* waveData;
	unsigned char* bufferPtr;
	unsigned long bufferSize;
	unsigned char* UseFile = file;

	memcpy((unsigned char*)&waveFileHeader, UseFile, sizeof(waveFileHeader)); ///  читаю описание файла

	// Open the wave file in binary.

	// Check that the chunk ID is the RIFF format.
	if((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') || 
	   (waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	// Check that the file format is the WAVE format.
	if((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
	   (waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	// Check that the sub chunk ID is the fmt format.
	if((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
	   (waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
	{
		return false;
	}

	// Check that the audio format is WAVE_FORMAT_PCM.
	if(waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
	{
		return false;
	}

	// Check that the wave file was recorded in stereo format.
	// 3D sound files must be recorded as single channel (mono). 
	if(waveFileHeader.numChannels != 1)
	{
		return false;
	}

	// Check that the wave file was recorded at a sample rate of 44.1 KHz.
	if(waveFileHeader.sampleRate != 44100)
	{
		return false;
	}

	// Ensure that the wave file was recorded in 16 bit format.
	if(waveFileHeader.bitsPerSample != 16)
	{
		return false;
	}

	// Check for the data chunk header.
	if((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
	   (waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
	{
		return false;
	}

	// Set the wave format of secondary buffer that this wave file will be loaded onto.
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 1; // 2 for NO 3D sound
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;



	// Create a temporary sound buffer with the specific buffer settings.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the temporary buffer.
	tempBuffer->Release();
	tempBuffer = 0;

	// Create a temporary buffer to hold the wave file data.
	waveData = new unsigned char[waveFileHeader.dataSize];
	if(!waveData)
	{
		return false;
	}

	// Read in the wave file data into the newly created buffer.
	memcpy(&waveData[0], UseFile, waveFileHeader.dataSize);
																			

	// Lock the secondary buffer to write wave data into it.
	result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if(FAILED(result))
	{
		return false;
	}

	// Copy the wave data into the buffer.
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	// Unlock the secondary buffer after the data has been written to it.
	result = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if(FAILED(result))
	{
		return false;
	}
	
	// Release the wave data since it was copied into the secondary buffer.
	delete [] waveData;
	waveData = 0;

// Now that the secondary sound buffer has the .wav file loaded into it we obtain a 3D interface to the secondary sound buffer.
// This interface will give us the 3D sound controls. However all other functions such as volume need to be set using the original secondary sound buffer interface. 

	// Get the 3D interface to the secondary sound buffer.
	result = (*secondaryBuffer)->QueryInterface(IID_IDirectSound3DBuffer8, (void**)&*secondary3DBuffer);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}


bool SoundClass::PlayWaveFile3D(UINT Index, float x, float y,float z,D3DVALUE dopler)
{
	HRESULT result;
//	float positionX, positionY, positionZ;

// Setup the position where we want the 3D sound to be located. In this case it will be set over to the left. 

	// Set the 3D position of where the sound should be located.
//	positionX = 0.0f;
//	positionY = 0.0f;
//	positionZ = 1.0f;


	// Set position at the beginning of the sound buffer.
	result = _3DSound[Index]->SetCurrentPosition(0);
	if(FAILED(result))
	{
		return false;
	}

	// Set volume of the buffer to 100%.
	result = _3DSound[Index]->SetVolume(EffectsVolume); //DSBVOLUME_MIN
	if(FAILED(result))
	{
		return false;
	}

// Now use the position and set the 3D sound position using the 3D interface. The DS3D_IMMEDIATE flag sets the sound position immediately instead of deferring it for later batch processing. 

	// Set the 3D position of the sound.
	_3DSoundControl[Index]->SetPosition(x, y, z, DS3D_IMMEDIATE);
	m_listener->SetDopplerFactor(dopler, DS3D_IMMEDIATE );
	// Play the contents of the secondary sound buffer.
	result = _3DSound[Index]->Play(0, 0, 0);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

// The LoadWaveFile function is what handles loading in a .wav audio file and then copies the data onto a new secondary buffer. If you are looking to do different formats you would replace this function or write a similar one.
bool SoundClass::LoadStereoWaveFile(unsigned char* file, IDirectSoundBuffer8** secondaryBuffer)
{

	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT result;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* waveData;
	unsigned char *bufferPtr;
	unsigned long bufferSize;
	unsigned char* UseFile = file;

// To start first open the .wav file and read in the header of the file. The header will contain all the information about the audio file so we can use that to create a secondary buffer to accommodate the audio data. The audio file header also tells us where the data begins and how big it is. You will notice I check for all the needed tags to ensure the audio file is not corrupt and is the proper wave file format containing RIFF, WAVE, fmt, data, and WAVE_FORMAT_PCM tags. I also do a couple other checks to ensure it is a 44.1KHz stereo 16bit audio file. If it is mono, 22.1 KHZ, 8bit, or anything else then it will fail ensuring we are only loading the exact format we want.


	memcpy((unsigned char*)&waveFileHeader, UseFile, sizeof(waveFileHeader)); ///  читаю описание файла
//	UseFile += sizeof(waveFileHeader);

 
	// Check that the chunk ID is the RIFF format.
	if((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') || 
	   (waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}
 
	// Check that the file format is the WAVE format.
	if((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
	   (waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}
 
	// Check that the sub chunk ID is the fmt format.
	if((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
	   (waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
	{
		return false;
	}
 
	// Check that the audio format is WAVE_FORMAT_PCM.
	if(waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
	{
		return false;
	}
 
	// Check that the wave file was recorded in stereo format.
	if(waveFileHeader.numChannels != 2)
	{
		return false;
	}
 
	// Check that the wave file was recorded at a sample rate of 44.1 KHz.
	if(waveFileHeader.sampleRate != 44100)
	{
		return false;
	}
 
	// Ensure that the wave file was recorded in 16 bit format.
	if(waveFileHeader.bitsPerSample != 16)
	{
		return false;
	}
 
	// Check for the data chunk header.
	if((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
	   (waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
	{
		return false;
	}

// Now that the wave header file has been verified we can setup the secondary buffer we will load the audio data onto. We have to first set the wave format and buffer description of the secondary buffer similar to how we did for the primary buffer. There are some changes though since this is secondary and not primary in terms of the dwFlags and dwBufferBytes.

	// Set the wave format of secondary buffer that this wave file will be loaded onto.
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;
 
	// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

// Now the way to create a secondary buffer is fairly strange. First step is that you create a temporary IDirectSoundBuffer with the sound buffer description you setup for the secondary buffer. If this succeeds then you can use that temporary buffer to create a IDirectSoundBuffer8 secondary buffer by calling QueryInterface with the IID_IDirectSoundBuffer8 parameter. If this succeeds then you can release the temporary buffer and the secondary buffer is ready for use.

	// Create a temporary sound buffer with the specific buffer settings.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if(FAILED(result))
	{
		return false;
	}
 
	// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if(FAILED(result))
	{
		return false;
	}
 
	// Release the temporary buffer.
	tempBuffer->Release();
	tempBuffer = 0;

// Now that the secondary buffer is ready we can load in the wave data from the audio file. First I load it into a memory buffer so I can check and modify the data if I need to. Once the data is in memory you then lock the secondary buffer, copy the data to it using a memcpy, and then unlock it. This secondary buffer is now ready for use. Note that locking the secondary buffer can actually take in two pointers and two positions to write to. This is because it is a circular buffer and if you start by writing to the middle of it you will need the size of the buffer from that point so that you don't write outside the bounds of it. This is useful for streaming audio and such. In this tutorial we create a buffer that is the same size as the audio file and write from the beginning to make things simple.

	// Move to the beginning of the wave data which starts at the end of the data chunk header.
	//fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);
 
	// Create a temporary buffer to hold the wave file data.
	waveData = new unsigned char[waveFileHeader.dataSize];
	if(!waveData)
	{
		return false;
	}
 

	memcpy(&waveData[0], UseFile, waveFileHeader.dataSize);
 

	// Lock the secondary buffer to write wave data into it.
	result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if(FAILED(result))
	{
		return false;
	}
 
	// Copy the wave data into the buffer.
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);
 
	// Unlock the secondary buffer after the data has been written to it.
	result = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if(FAILED(result))
	{
		return false;
	}
	
	// Release the wave data since it was copied into the secondary buffer.
	delete [] waveData;
	waveData = 0;
 
	return true;
}


bool SoundClass::LoadOggMusicFile(unsigned char* file, IDirectSoundBuffer8** secondaryBuffer , int size)
{
//  VORBIS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	HRESULT Res;
	stb_vorbis *VorbisOgg;
	stb_vorbis_info VorbisInfo;

	stb_vorbis_alloc *VorbisBufferDesc = new stb_vorbis_alloc;
	VorbisBufferDesc->alloc_buffer = new char[10000000];
	VorbisBufferDesc->alloc_buffer_length_in_bytes = 10000000; 

	int *Error = new int;
	*Error = -1;


	VorbisOgg = stb_vorbis_open_memory(file, size, Error, VorbisBufferDesc);

	VorbisInfo = stb_vorbis_get_info( VorbisOgg );

	UINT SamplesNumber = stb_vorbis_stream_length_in_samples( VorbisOgg );

	UINT TotalMemoryRequired = SamplesNumber * VorbisInfo.channels;// * VorbisInfo.sample_rate;

	short *a = new short[TotalMemoryRequired];

//	int ChanelSamples = stb_vorbis_get_samples_short_interleaved( VorbisOgg, VorbisInfo.channels, a, TotalMemoryRequired);

	stb_vorbis_close( VorbisOgg );

//	UINT8 *Mem = new UINT8 [ChanelSamples];
	int Resu = stb_vorbis_decode_memory(file , size , &VorbisInfo.channels , &VorbisInfo.sample_rate, &a);

	IDirectSoundBuffer* tempBuffer;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	unsigned char *bufferPtr;
	unsigned long bufferSize1;

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = VorbisInfo.sample_rate;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = VorbisInfo.channels;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;
 
	// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;// | DSBCAPS_CTRL3D;
	bufferDesc.dwBufferBytes = SamplesNumber;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

// Now the way to create a secondary buffer is fairly strange. First step is that you create a temporary IDirectSoundBuffer with the sound buffer description you setup for the secondary buffer. If this succeeds then you can use that temporary buffer to create a IDirectSoundBuffer8 secondary buffer by calling QueryInterface with the IID_IDirectSoundBuffer8 parameter. If this succeeds then you can release the temporary buffer and the secondary buffer is ready for use.

	// Create a temporary sound buffer with the specific buffer settings.
	Res = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if(FAILED(Res))
	{
		return false;
	}
 
	// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
	Res = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if(FAILED(Res))
	{
		return false;
	}
 
	// Release the temporary buffer.
	tempBuffer->Release();
	tempBuffer = 0;

	Res = (*secondaryBuffer)->Lock(0, SamplesNumber, (void**)&bufferPtr, (DWORD*)&bufferSize1, NULL, 0, 0);
	if(FAILED(Res))
	{
		return false;
	}
 
	// Copy the wave data into the buffer.
	memcpy(bufferPtr, a, SamplesNumber);
 
	// Unlock the secondary buffer after the data has been written to it.
	Res = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize1, NULL, 0);
	if(FAILED(Res))
	{
		return false;
	}

	stb_vorbis_close( VorbisOgg );
//	delete Mem;
	delete [] a;
	delete Error;
	delete VorbisBufferDesc->alloc_buffer;
	delete VorbisBufferDesc;
//  VORBIS -------------------------------------------------------------
	return true;
}

// The PlayWaveFile function will play the audio file stored in the secondary buffer. The moment you use the Play function it will automatically mix the audio into the primary buffer and start it playing if it wasn't already. Also note that we set the position to start playing at the beginning of the secondary sound buffer otherwise it will continue from where it last stopped playing. And since we set the capabilities of the buffer to allow us to control the sound we set the volume to maximum here.
bool SoundClass::PlayWaveFile(UINT Index)
{
	HRESULT result;
 
 
	// Set position at the beginning of the sound buffer.
	result = SoundStereo[Index]->SetCurrentPosition(0);
	if(FAILED(result))
	{
		return false;
	}
 
	// Set volume of the buffer to 100%.
	result = SoundStereo[Index]->SetVolume(MusicVolume);
	if(FAILED(result))
	{
		return false;
	}
 
	// Play the contents of the secondary sound buffer.
	result = SoundStereo[Index]->Play(0, 0, 0);
	if(FAILED(result))
	{
		return false;
	}
 
	return true;
}


 bool SoundClass::StereoSoundPlaying( UINT Index )
 {
	SoundStereo[Index]->GetStatus(BuffersStatus);

	if ( *BuffersStatus == DSBSTATUS_PLAYING )
		return true;

	return false;
 }


 bool SoundClass::_3DSoundPlaying( UINT Index )
 {
	_3DSound[Index]->GetStatus(BuffersStatus);

	if ( *BuffersStatus == DSBSTATUS_PLAYING )
		return true;

	return false;
 }


 bool SoundClass::OggMusicPlaying( UINT Index )
 {
	SoundStereo[Index]->GetStatus(BuffersStatus);

	if ( *BuffersStatus == DSBSTATUS_PLAYING )
		return true;

	return false;
 }


void SoundClass::SetEffectVolume( UINT Index, long Volume )
{
	_3DSound[Index]->SetVolume(Volume);
}


void SoundClass::SetMusicVolume( UINT Index, long Volume )
{
	SoundStereo[Index]->SetVolume(Volume);
}


void SoundClass::SetEffectsOFF()
{
	UINT Num = (UINT)_3DSound.size();
	for ( UINT i = 0 ; i < Num ; ++i )
	_3DSound[i]->SetVolume(DSBVOLUME_MIN);
}


void SoundClass::SetMusicOFF()
{
	UINT Num = (UINT)_3DSound.size();
	for ( UINT i = 0 ; i < Num ; ++i )
	SoundStereo[i]->SetVolume(DSBVOLUME_MIN);
}
#include "stdafx.h"

// The FpsClass is simply a counter with a timer associated with it. It counts how many frames occur in a one second period and constantly updates that count.
////////////////////////////////////////////////////////////////////////////////
// Filename: fpsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _FPSCLASS_H_
#define _FPSCLASS_H_


/////////////
// LINKING //
/////////////
#pragma comment(lib, "winmm.lib")


//////////////
// INCLUDES //
//////////////
#include <windows.h>
#include <mmsystem.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: FpsClass
////////////////////////////////////////////////////////////////////////////////
class FpsClass
{
public:
	FpsClass();
	FpsClass(const FpsClass&);
	~FpsClass();

	void Initialize();
	void Frame();
	void GetFps(UINT&);

private:
	int m_fps;
	int m_count;
	unsigned long m_startTime;
};

#endif

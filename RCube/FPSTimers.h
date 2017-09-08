#pragma once
#include "stdafx.h"

#ifndef FPSTIMERS_H_
#define FPSTIMERS_H_

// ќбъ€вление глобальной структуры в которую сохран€ютс€ значени€ счЄтчиков и таймеров движка 
// дл€ передачи в Graphics.h - вывод на экран FPS и CPU
struct FPSTimers {

	UINT CpuVal;
	int FpsRate;
	int ComputerSpeed;
   float FrameTime;
    UINT Seed;

};

#endif
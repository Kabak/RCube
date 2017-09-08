#pragma once
#include "stdafx.h"

#include <time.h>
#include <stdio.h>

#ifndef _RCUBEPROFILEING_H_
#define _RCUBEPROFILEING_H_


class RCubeProfiling
{
public:

	RCubeProfiling();
   ~RCubeProfiling();

	void Initialize();
	void StartTimer();
	void StopTimer(float&);
	void StopTimer(char*);

	char *String;

private:
	// Фиксированная системная переменная - частота процессора тактов в секунду.
	LARGE_INTEGER m_frequency;
	// Количество процессорных тактов в милисекунду
//	__int64 m_ticksPerMs;
	// Количество процессорных тактов в секунду
	__int64 m_ticksPerSec;

	// Переменные для таймера скорости выполнения
	LARGE_INTEGER StartCounter;
	LARGE_INTEGER StopCounter;
	LARGE_INTEGER ResultCounter;
	LARGE_INTEGER OutCounter;
			float TimeAmount;
			float TickTime;
};

#endif
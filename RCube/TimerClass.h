#pragma once
#include "stdafx.h"

#ifndef _TIMERCLASS_H_
#define _TIMERCLASS_H_

#include <time.h>
#include <stdio.h>

class TimerClass
{
public:
	TimerClass();
	TimerClass(const TimerClass&);
	~TimerClass();

	bool Initialize();
	void Frame();

// Получить сколько времени прошло в секундас между моментами времени
	void ComputeTimePassed(float& result, LARGE_INTEGER startTime, LARGE_INTEGER stopTime );

	void GetFps(int&);

// Функции для тайминга функций и определения быстродействия участков кода
	void StartTimer();
	void StopTimer(float&);

// Индивидуальный таймер
	void StartTimer(LARGE_INTEGER&Counter);
	void StopTimer(char* Result);
	void StopTimer(char* Result, LARGE_INTEGER& startcounter, LARGE_INTEGER& stopcounter);
// Индивидуальный таймер

// Random generator
	void GenRandUInt (int&, int);

	int GenerateRandNym(int MinValue , int MaxValue);

	char *String;

private:
	void GenString(char* Result, float& TimeAmount);

// Фиксированная системная переменная - частота процессора тактов в секунду.
	LARGE_INTEGER m_frequency;
	LARGE_INTEGER currentTime;
// начало отсчёта фрейма	
	LARGE_INTEGER startTime;

// Количество процессорных тактов в секунду
	__int64 m_ticksPerSec;

// Количесто процессорных тактов в одном кадре
	__int64 frameTime;

// техническое значение кадров в секунду
	   UINT frameCount;

// Количество кадров в секунду
	   UINT m_fps;

// Времени за один такт CPU в наносекундах
	float TickTime_Sec; // Времени за один такт в секундах
	float TickTime_ns;	// Времени за один такт в наносекундах
	float TickTime_mks;	// Времени за один такт в микросекундах

// Переменные для таймера скорости выполнения
	LARGE_INTEGER StartCounter;
	LARGE_INTEGER StopCounter;
	LARGE_INTEGER ResultCounter;
	LARGE_INTEGER OutCounter;
			float TimeAmount;

};

#endif

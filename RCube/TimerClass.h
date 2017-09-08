#pragma once
#include "stdafx.h"
// The TimerClass is a high precision timer that measures the exact time between frames of execution. Its primary use is for synchronizing objects that require a standard time frame for movement. 
// In this tutorial we won't have a use for it but we will implement it in the code so you can see how to apply it to your projects. The most common usage of the TimerClass is to use the frame 
// time to figure out what percentage of a second has passed in the current frame and then move the objects by that percentage.

////////////////////////////////////////////////////////////////////////////////
// Filename: timerclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TIMERCLASS_H_
#define _TIMERCLASS_H_


//////////////
// INCLUDES //
//////////////
//#include <windows.h>
#include <time.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
// Class name: TimerClass
////////////////////////////////////////////////////////////////////////////////
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
// Получить текучее значение тактов
	void GetTicks(LARGE_INTEGER& Ticks);

	void GetFrameTime(float&);
	void GetFps(int&);
	void GetSpeed(int&);

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
	   UINT m_computer_speed;

// Времени за один такт CPU в наносекундах
	float TickTime;
	float TickTimeSec;
// Переменные для таймера скорости выполнения
	LARGE_INTEGER StartCounter;
	LARGE_INTEGER StopCounter;
	LARGE_INTEGER ResultCounter;
	LARGE_INTEGER OutCounter;
			float TimeAmount;

};

#endif

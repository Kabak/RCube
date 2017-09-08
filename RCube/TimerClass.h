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

// �������� ������� ������� ������ � �������� ����� ��������� �������
	void ComputeTimePassed(float& result, LARGE_INTEGER startTime, LARGE_INTEGER stopTime );
// �������� ������� �������� ������
	void GetTicks(LARGE_INTEGER& Ticks);

	void GetFrameTime(float&);
	void GetFps(int&);
	void GetSpeed(int&);

// ������� ��� �������� ������� � ����������� �������������� �������� ����
	void StartTimer();
	void StopTimer(float&);

// �������������� ������
	void StartTimer(LARGE_INTEGER&Counter);
	void StopTimer(char* Result);
	void StopTimer(char* Result, LARGE_INTEGER& startcounter, LARGE_INTEGER& stopcounter);
// �������������� ������

// Random generator
	void GenRandUInt (int&, int);

	int GenerateRandNym(int MinValue , int MaxValue);

	char *String;

private:
	void GenString(char* Result, float& TimeAmount);

// ������������� ��������� ���������� - ������� ���������� ������ � �������.
	LARGE_INTEGER m_frequency;
	LARGE_INTEGER currentTime;
// ������ ������� ������	
	LARGE_INTEGER startTime;

// ���������� ������������ ������ � �������
	__int64 m_ticksPerSec;

// ��������� ������������ ������ � ����� �����
	  __int64 frameTime;

// ����������� �������� ������ � �������
	   UINT frameCount;

// ���������� ������ � �������
	   UINT m_fps;
	   UINT m_computer_speed;

// ������� �� ���� ���� CPU � ������������
	float TickTime;
	float TickTimeSec;
// ���������� ��� ������� �������� ����������
	LARGE_INTEGER StartCounter;
	LARGE_INTEGER StopCounter;
	LARGE_INTEGER ResultCounter;
	LARGE_INTEGER OutCounter;
			float TimeAmount;

};

#endif

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

// �������� ������� ������� ������ � �������� ����� ��������� �������
	void ComputeTimePassed(float& result, LARGE_INTEGER startTime, LARGE_INTEGER stopTime );

	void GetFps(int&);

// ������� ��� �������� ������� � ����������� �������������� �������� ����
	void StartTimer();
	void StopTimer(float&);

// �������������� ������
	void StartTimer(LARGE_INTEGER&Counter);
	void StopTimer(char* Result);
	void StopTimer(char* Result, LARGE_INTEGER& startcounter, LARGE_INTEGER& stopcounter);
// �������������� ������

// Random generator
   float GenerateRandFloatRegion (float& Max, float& Min);
	 int GenerateRandIntRegion (int& Max, int& Min);

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

// Random generator Values
	__int64 BigBangTime;	// Engine start time - constant
	__int64 Time1;			// Start Time
	__int64 Time2;			// FrameTime
	__int64 Time3;			// Temp value
		int RandomInt;		// Random Value
		int LastInt;
	  float RandomFloat;	// Random Value
	  float LastFloat;
	   bool Odd;			//
	   bool Divede3;
	   bool Divede5;
	    int Algo;

// ����������� �������� ������ � �������
	    int frameCount;

// ���������� ������ � �������
	    int m_fps;

// ������� �� ���� ���� CPU � ������������
	float TickTime_Sec; // ������� �� ���� ���� � ��������
	float TickTime_ns;	// ������� �� ���� ���� � ������������
	float TickTime_mks;	// ������� �� ���� ���� � �������������

// ���������� ��� ������� �������� ����������
	LARGE_INTEGER StartCounter;
	LARGE_INTEGER StopCounter;
	LARGE_INTEGER ResultCounter;
	LARGE_INTEGER TempCounter;
			float TimeAmount;

};

#endif

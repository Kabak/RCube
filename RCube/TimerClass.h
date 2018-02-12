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

// ������� �� ���� ���� CPU � ������������
	float TickTime_Sec; // ������� �� ���� ���� � ��������
	float TickTime_ns;	// ������� �� ���� ���� � ������������
	float TickTime_mks;	// ������� �� ���� ���� � �������������

// ���������� ��� ������� �������� ����������
	LARGE_INTEGER StartCounter;
	LARGE_INTEGER StopCounter;
	LARGE_INTEGER ResultCounter;
	LARGE_INTEGER OutCounter;
			float TimeAmount;

};

#endif

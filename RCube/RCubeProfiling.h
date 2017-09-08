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
	// ������������� ��������� ���������� - ������� ���������� ������ � �������.
	LARGE_INTEGER m_frequency;
	// ���������� ������������ ������ � �����������
//	__int64 m_ticksPerMs;
	// ���������� ������������ ������ � �������
	__int64 m_ticksPerSec;

	// ���������� ��� ������� �������� ����������
	LARGE_INTEGER StartCounter;
	LARGE_INTEGER StopCounter;
	LARGE_INTEGER ResultCounter;
	LARGE_INTEGER OutCounter;
			float TimeAmount;
			float TickTime;
};

#endif
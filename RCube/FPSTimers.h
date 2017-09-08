#pragma once
#include "stdafx.h"

#ifndef FPSTIMERS_H_
#define FPSTIMERS_H_

// ���������� ���������� ��������� � ������� ����������� �������� ��������� � �������� ������ 
// ��� �������� � Graphics.h - ����� �� ����� FPS � CPU
struct FPSTimers {

	UINT CpuVal;
	int FpsRate;
	int ComputerSpeed;
   float FrameTime;
    UINT Seed;

};

#endif
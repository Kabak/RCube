#include "stdafx.h"

#include "RCubeProfiling.h"

RCubeProfiling::RCubeProfiling()
{
	m_frequency.QuadPart = 0;
		   m_ticksPerSec = 0;

				  String = new char[16];
			   String[0] = NULL;
}


RCubeProfiling::~RCubeProfiling()
{
	delete [] String;
}


void RCubeProfiling::Initialize()
{
	// Check to see if this system supports high performance timers.
	QueryPerformanceFrequency(&m_frequency);

	// Find out how many times the frequency counter ticks every millisecond.
//	m_ticksPerMs = m_frequency.QuadPart / 1000;
	m_ticksPerSec = (m_frequency.QuadPart);
//	TickTime = (double) 1 / m_ticksPerSec * 1000000; // Времени за один такт в микросекундах
	TickTime = (float)1 / m_ticksPerSec * 1000000000; // Времени за один такт в наносекундах
}


// Функции для тайминга функций и определения быстродействия участков кода
void RCubeProfiling::StartTimer()
{
	QueryPerformanceCounter(&StartCounter);
}


void RCubeProfiling::StopTimer(float& Result)
{
	QueryPerformanceCounter(&StopCounter);
	// https://msdn.microsoft.com/en-us/library/dn553408%28v=vs.85%29.aspx
	ResultCounter.QuadPart = StopCounter.QuadPart - StartCounter.QuadPart; //    / m_ticksPerMs      (m_ticksPerSec / 1000 )

	Result = (float)ResultCounter.QuadPart * TickTime;
	ResultCounter.QuadPart = 0;

}


void RCubeProfiling::StopTimer(char* Result)
{
	QueryPerformanceCounter(&StopCounter);
	// https://msdn.microsoft.com/en-us/library/dn553408%28v=vs.85%29.aspx
	ResultCounter.QuadPart = StopCounter.QuadPart - StartCounter.QuadPart; //    / m_ticksPerMs      (m_ticksPerSec / 1000 )

	TimeAmount = (float)ResultCounter.QuadPart * TickTime;
	ResultCounter.QuadPart = 0;

	if (TimeAmount > 1000000000)
	{
		TimeAmount /= 1000000000;
		sprintf_s(String, 16, "%8.2f sec", TimeAmount);
	}
	else if (TimeAmount > 1000000)
	{
		TimeAmount /= 1000000;
		sprintf_s(String, 16, "%8.2f ms", TimeAmount);
	}
	else if (TimeAmount > 1000)
	{
		TimeAmount /= 1000;
		sprintf_s(String, 16, "%8.2f mks", TimeAmount);
	}
	else if (TimeAmount > 1)
	{
		sprintf_s(String, 16, "%8.2f ns", TimeAmount);
	}

	strcpy_s(Result,16,String);
}

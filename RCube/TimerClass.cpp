#include "stdafx.h"
///////////////////////////////////////////////////////////////////////////////
// Filename: timerclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "timerclass.h"


TimerClass::TimerClass()
{
 m_frequency.QuadPart = 0;
 currentTime.QuadPart = 0;
   startTime.QuadPart = 0;
		m_ticksPerSec = 0;
			 TickTime = 0.0f;
		   TickTimeSec = 0.0f;
		    frameTime = 0;
		   frameCount = 0;
				m_fps = 0;

				String = new char[16];
			 String[0] = NULL;

//srand(static_cast<unsigned int> ( time(0) ));
srand( (UINT)time( NULL ) );
}


TimerClass::TimerClass(const TimerClass& other)
{
	delete[] String;
}


TimerClass::~TimerClass()
{
}


bool TimerClass::Initialize()
{
	// Check to see if this system supports high performance timers.
	QueryPerformanceFrequency(&m_frequency);

	// Find out how many times the frequency counter ticks every millisecond.
//	m_ticksPerMs = m_frequency.QuadPart / 1000;
	// Find out how many times the frequency counter ticks every second.
	m_ticksPerSec = (m_frequency.QuadPart);

	TickTimeSec = (float) 1 / m_ticksPerSec * 1000000;// * 1000000; // Времени за один такт в микросекундах
	TickTime = (float) 1 / m_ticksPerSec * 1000000000; // Времени за один такт в наносекундах

	QueryPerformanceCounter(&startTime);

	srand((unsigned int)time(0));

	return true;
}


void TimerClass::Frame()
{

	QueryPerformanceCounter(&currentTime);

//	frameTimeOld = currentTime.QuadPart;
//	https://msdn.microsoft.com/en-us/library/dn553408%28v=vs.85%29.aspx

	frameTime = (currentTime.QuadPart - startTime.QuadPart) * 1000000000 / m_ticksPerSec;

	++frameCount;

	if ( frameTime > 1000000000LL )
	{
		m_fps = frameCount;
// Вычисляем скорость отрисовки для качественной прорисовки движущихся объектов
		m_computer_speed = frameCount / 5;
		frameCount = 0;
		startTime = currentTime;
	}	

	return;
}

// GetTime returns the most recent frame time that was calculated.


void TimerClass::ComputeTimePassed(float & result, LARGE_INTEGER startTime, LARGE_INTEGER stopTime)
{
	QueryPerformanceCounter(&stopTime);
	result = (float)((stopTime.QuadPart - startTime.QuadPart) * TickTimeSec / (1000000 * 60)); 
}

void TimerClass::GetTicks(LARGE_INTEGER & Ticks)
{
	QueryPerformanceCounter(&Ticks);
}

void TimerClass::GetFrameTime(float &FrameTime)
{
	// Одну секунду делим на количество фреймов в секунду
	// получаем сколько времени нужно на один фрейм
	FrameTime = 1.0f / m_fps;

	return;
}

void TimerClass::GetFps(int& FpsRate)
{
	FpsRate = (int)m_fps;

	return; 
}

void TimerClass::GetSpeed(int& CompSpeed)
{
	CompSpeed = (int)m_computer_speed;

	return; 
}


void TimerClass::StartTimer()
{
	QueryPerformanceCounter(&StartCounter);
}


void TimerClass::StopTimer(float& Result)
{
	QueryPerformanceCounter(&StopCounter);
	Result = (float)((StopCounter.QuadPart - StartCounter.QuadPart) / m_ticksPerSec);
}


void TimerClass::GenRandUInt(int& data, int range)
{
	data = rand() % range ;
}

int TimerClass::GenerateRandNym(int MinValue, int MaxValue) {

	int Result = 0 ;

	Result = ((rand() % MaxValue * 2 * 10) / 10) + MinValue;

	return Result;
}

// Индивидуальный таймер
void TimerClass::StartTimer(LARGE_INTEGER& Counter)
{
	QueryPerformanceCounter(&Counter);
}


void TimerClass::StopTimer(char* Result)
{
	QueryPerformanceCounter(&StopCounter);
	// https://msdn.microsoft.com/en-us/library/dn553408%28v=vs.85%29.aspx
	ResultCounter.QuadPart = StopCounter.QuadPart - StartCounter.QuadPart; //    / m_ticksPerMs      (m_ticksPerSec / 1000 )

//	OutCounter = ResultCounter;
	TimeAmount = (float)ResultCounter.QuadPart * TickTime;
//	ResultCounter.QuadPart = 0;
	GenString(Result, TimeAmount );
}


void TimerClass::StopTimer(char* Result, LARGE_INTEGER& startcounter, LARGE_INTEGER& stopcounter)
{
	QueryPerformanceCounter(&stopcounter);
	// https://msdn.microsoft.com/en-us/library/dn553408%28v=vs.85%29.aspx
	ResultCounter.QuadPart = stopcounter.QuadPart - startcounter.QuadPart;

	TimeAmount = (float)ResultCounter.QuadPart * TickTime;
	GenString(Result, TimeAmount);
}


void TimerClass::GenString(char* Result, float& TimeAmount)
{
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

	strcpy_s(Result, 16, String);
}
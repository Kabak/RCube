#include "stdafx.h"

#include "timerclass.h"


TimerClass::TimerClass()
{
 m_frequency.QuadPart = 0;
 currentTime.QuadPart = 0;
   startTime.QuadPart = 0;
		m_ticksPerSec = 0;
		 TickTime_Sec = 0.0f;
		  TickTime_ns = 0.0f;
		 TickTime_mks = 0.0f;
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

	TickTime_Sec = ( float ) 1 / m_ticksPerSec;				// Времени за один такт в секундах
	TickTime_ns = (float) 1 / m_ticksPerSec * 1000000000;	// Времени за один такт в наносекундах
	TickTime_mks = ( float ) 1 / m_ticksPerSec * 1000000;	// Времени за один такт в микросекундах

	QueryPerformanceCounter(&startTime);

	BigBangTime = startTime.QuadPart;
//	srand((unsigned int)time(0));

	return true;
}


void TimerClass::Frame()
{

	QueryPerformanceCounter(&currentTime);

//	https://msdn.microsoft.com/en-us/library/dn553408%28v=vs.85%29.aspx

	frameTime = (currentTime.QuadPart - startTime.QuadPart) * 1000000000 / m_ticksPerSec;	// Время кадра в наносекундах

	++frameCount;

	if ( frameTime > 1000000000LL )
	{
		m_fps = frameCount;
		frameCount = 0;
		startTime = currentTime;
	}	

	return;
}


void TimerClass::ComputeTimePassed(float & result, LARGE_INTEGER startTime, LARGE_INTEGER stopTime)
{
	QueryPerformanceCounter(&stopTime);
	result = ( float ) ( ( stopTime.QuadPart - startTime.QuadPart ) * TickTime_Sec );
}


void TimerClass::GetFps(int& FpsRate)
{
	FpsRate = m_fps;

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


float TimerClass::GenerateRandFloatRegion (float& Max, float& Min)
{
	QueryPerformanceCounter (&TempCounter);
/*
	__int64 Temp = TempCounter.QuadPart;

	Temp % 2 ? Odd = false : Odd = true;
	Temp % 3 ? Divede3 = false : Divede3 = true;
	Temp % 5 ? Divede5 = false : Divede5 = true;

	Algo = static_cast < int > ( Temp & 0x0000000f) ;

	LastFloat = static_cast < float > ( TempCounter.QuadPart );

	LastFloat = RandomFloat = Min + static_cast < float > ( static_cast < float > ( Temp / ( Max - Min ) ));
*/
	srand (TempCounter.LowPart);
//	while ( Result < Min )
//		Result = Min + static_cast <float> ( rand () ) / ( static_cast <float> ( RAND_MAX / ( Max - Min ) ) );

	return RandomFloat = 0.0f;
}


int  TimerClass::GenerateRandIntRegion (int& Max, int& Min)
{
	int Result = Min - 1;

	while ( Result < Min )
		Result = rand () % Max;

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
	ResultCounter.QuadPart = StopCounter.QuadPart - StartCounter.QuadPart;

	TimeAmount = (float)ResultCounter.QuadPart * TickTime_ns;
	GenString(Result, TimeAmount );
}


void TimerClass::StopTimer(char* Result, LARGE_INTEGER& startcounter, LARGE_INTEGER& stopcounter)
{
	QueryPerformanceCounter(&stopcounter);
	// https://msdn.microsoft.com/en-us/library/dn553408%28v=vs.85%29.aspx
	ResultCounter.QuadPart = stopcounter.QuadPart - startcounter.QuadPart;

	TimeAmount = (float)ResultCounter.QuadPart * TickTime_ns;
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
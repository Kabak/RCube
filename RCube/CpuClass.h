#pragma once
#include "stdafx.h"
// The CpuClass is used to determine the percentage of total cpu use that is occurring each second.
///////////////////////////////////////////////////////////////////////////////
// Filename: cpuclass.h
///////////////////////////////////////////////////////////////////////////////
#ifndef _CPUCLASS_H_
#define _CPUCLASS_H_

// We use the pdh library to query the cpu usage.

/////////////
// LINKING //
/////////////
#pragma comment(lib, "pdh.lib")


//////////////
// INCLUDES //
//////////////
#include <pdh.h>


///////////////////////////////////////////////////////////////////////////////
// Class name: CpuClass
///////////////////////////////////////////////////////////////////////////////
class CpuClass
{
public:
	CpuClass();
	CpuClass(const CpuClass&);
	~CpuClass();

	void Initialize();
	void Shutdown();
	void Frame();
	void GetCpuPercentage(UINT&);

private:
	bool m_canReadCpu;
	HQUERY m_queryHandle;
	HCOUNTER m_counterHandle;
	unsigned long long m_lastSampleTime;
	unsigned long long m_cpuUsage;
};

#endif
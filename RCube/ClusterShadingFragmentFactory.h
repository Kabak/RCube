#pragma once
#include "stdafx.h"
#include <vector>

#ifndef _FRAGMENTFACTORY_H_
#define _FRAGMENTFACTORY_H_

using namespace std;

class FragmentFactory
{
	vector<uint64_t> masks;

public:
	FragmentFactory();
	uint64_t coverage(int& x1, int& x2, int& y1, int& y2, int& z1, int& z2) const;
};

#endif
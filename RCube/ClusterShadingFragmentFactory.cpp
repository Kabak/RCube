// Copyright 2014 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.

#include "stdafx.h"
#include "ClusterShadingFragmentFactory.h"


FragmentFactory::FragmentFactory()
{
	masks.resize(48);

	for (int k = 0; k < 16; ++k)
	{
		int b = k % 4;
		int a = k / 4;

		uint64_t one = 1;
		uint64_t x_segment = 0;
		uint64_t y_segment = 0;
		uint64_t z_segment = 0;

		for (int l = a; l <= b; ++l)
			for (int m = 0; m < 16; ++m)
			{
				x_segment |= one << ((l / 2 % 2 + m / 8 % 2 * 2) * 16 + (l % 2 + m / 4 % 2 * 2) * 4 + (m % 4));
				y_segment |= one << ((l / 2 % 2 * 2 + m / 8 % 2) * 16 + (l % 2 * 2 + m / 4 % 2) * 4 + (m % 4));
				z_segment |= one << (m * 4 + l);
			}

		masks[0 + k] = x_segment;
		masks[16 + k] = y_segment;
		masks[32 + k] = z_segment;
	}
}

uint64_t FragmentFactory::coverage(int& x1, int& x2, int& y1, int& y2, int& z1, int& z2) const
{
	uint64_t x_segment = masks[0 + x1 * 4 + x2];
	uint64_t y_segment = masks[16 + y1 * 4 + y2];
	uint64_t z_segment = masks[32 + z1 * 4 + z2];
	uint64_t coverage = x_segment & y_segment & z_segment;
	return coverage;
}

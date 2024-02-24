#include "RadixSort.h"

#include <stdint.h>
#include <algorithm>
#include <random>

// ---- use SSE prefetch (needs compiler support), not really a problem on non-SSE machines.
//		need http://msdn.microsoft.com/vstudio/downloads/ppack/default.asp
//		or recent VC to use this
#define PREFETCH 1

#if PREFETCH
#include <xmmintrin.h>	// for prefetch
#define pfval	64
#define pfval2	128
#define pf(x)	_mm_prefetch((const char*)(x + i + pfval), 0)
#define pf2(x)	_mm_prefetch((const char*)(x + i + pfval2), 0)
#else
#define pf(x)
#define pf2(x)
#endif


// ------------------------------------------------------------------------------------------------
// ---- Visual C++ eccentricities

#if _WINDOWS
#define finline __forceinline
#else
#define finline inline
#endif

namespace engine::general
{
	// ================================================================================================
	// flip a float for sorting
	//  finds SIGN of fp number.
	//  if it's 1 (negative float), it flips all bits
	//  if it's 0 (positive float), it flips the sign only
	// ================================================================================================
	uint32_t FloatFlip(uint32_t f)
	{
		uint32_t mask = -int32_t(f >> 31) | 0x80000000;
		return f ^ mask;
	}

	void FloatFlipX(uint32_t& f)
	{
		uint32_t mask = -int32_t(f >> 31) | 0x80000000;
		f ^= mask;
	}

	// ================================================================================================
	// flip a float back (invert FloatFlip)
	//  signed was flipped from above, so:
	//  if sign is 1 (negative), it flips the sign bit back
	//  if sign is 0 (positive), it flips all bits back
	// ================================================================================================
	uint32_t IFloatFlip(uint32_t f)
	{
		uint32_t mask = ((f >> 31) - 1) | 0x80000000;
		return f ^ mask;
	}

	// ---- utils for accessing 11-bit quantities
#define _0(x)	(x & 0x7FF)
#define _1(x)	(x >> 11 & 0x7FF)
#define _2(x)	(x >> 22 )

	// ================================================================================================
	// Main radix sort
	// ================================================================================================
	void RadixSort(std::vector<float>& farray)
	{
		uint32_t elements = farray.size();
		std::vector<float> sorted(elements);

		uint32_t i;
		uint32_t* sort = (uint32_t*)sorted.data();
		uint32_t* array = (uint32_t*)farray.data();

		// 3 histograms on the stack:
		const uint32_t kHist = 2048;
		std::vector<uint32_t> b0(kHist * 3, 0);

		uint32_t* b1 = b0.data() + kHist;
		uint32_t* b2 = b1 + kHist;

		// 1.  parallel histogramming pass
		for (i = 0; i < elements; i++) {
			pf(array);
			uint32_t fi = FloatFlip((uint32_t&)array[i]);
			b0[_0(fi)] ++;
			b1[_1(fi)] ++;
			b2[_2(fi)] ++;
		}

		// 2.  Sum the histograms -- each histogram entry records the number of values preceding itself.
		{
			uint32_t sum0 = 0, sum1 = 0, sum2 = 0;
			uint32_t tsum;
			for (i = 0; i < kHist; i++) {
				tsum = b0[i] + sum0;
				b0[i] = sum0 - 1;
				sum0 = tsum;

				tsum = b1[i] + sum1;
				b1[i] = sum1 - 1;
				sum1 = tsum;

				tsum = b2[i] + sum2;
				b2[i] = sum2 - 1;
				sum2 = tsum;
			}
		}

		// byte 0: floatflip entire value, read/write histogram, write out flipped
		for (i = 0; i < elements; i++) {
			pf2(array);
			uint32_t fi = array[i];
			FloatFlipX(fi);
			uint32_t pos = _0(fi);
			sort[++b0[pos]] = fi;
		}

		// byte 1: read/write histogram, copy
		//   sorted -> array
		for (i = 0; i < elements; i++) {
			pf2(sort);
			uint32_t si = sort[i];
			uint32_t pos = _1(si);
			array[++b1[pos]] = si;
		}

		// byte 2: read/write histogram, copy & flip out
		//   array -> sorted
		for (i = 0; i < elements; i++) {
			pf2(array);
			uint32_t ai = array[i];
			uint32_t pos = _2(ai);
			sort[++b2[pos]] = IFloatFlip(ai);
		}

		// to write original:
		std::copy(sorted.begin(), sorted.end(), farray.begin());
	}

}
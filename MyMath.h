#pragma once

#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#undef min
#undef max
#include <algorithm>
#include "MyMisc.h"

// min of 3 elements
template <class T>
inline T min3(const T x0, const T x1, const T x2)
{ return std::min(std::min(x0, x1), x2); }

inline double clampAbs(double _u, double _v)
{
	unsigned __int64 u = (unsigned __int64 &)_u;
	unsigned __int64 v = (unsigned __int64 &)_v;
	unsigned __int64 sign = u & 0x8000000000000000L;
	u ^= sign;
	u = std::min(u, v);
	u |= sign;
	return (double &)u;
}
inline float clampAbs(float _u, float _v)
{
	unsigned u = (unsigned &)_u;
	unsigned v = (unsigned &)_v;
	unsigned sign = u & 0x80000000L;
	u ^= sign;
	u = std::min(u, v);
	u |= sign;
	return (float &)u;
}

inline double sqr(double f)
{
	return f * f;
}
inline NvU32 isqr(int i)
{
    return i * i;
}
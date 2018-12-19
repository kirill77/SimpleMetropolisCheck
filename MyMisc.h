#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <memory.h>

#ifdef NELEMENTS
#undef NELEMENTS
#endif

typedef unsigned __int64 NvU64;
typedef unsigned NvU32;
typedef unsigned short NvU16;

const int MAX_INT = (int)0x7fffffff;
const NvU32 MAX_UINT = 0xffffffffU;
const double MAX_FLOAT = 1e38f;

#define NELEMENTS(x) (sizeof(x)/sizeof(x[0]))

#ifdef _DEBUG
#define ASSERT_ONLY_CODE 1
#endif

#if ASSERT_ONLY_CODE
inline void nvAssert(unsigned condition)
{
    if (!condition)
    {
        __debugbreak();
    }
}
#else
#define nvAssert(x)
#endif

#define NVCTASSERT(b) static_assert((b), "static assert failed");

#define NV_ALIGN_UP(x, ALIGNMENT) (((x + ALIGNMENT - 1) / ALIGNMENT) * ALIGNMENT)

#define ARRAY_ELEMENT_COUNT(x) (sizeof(x) / sizeof((x)[0]))

// make n steps to the right from f
inline float next_float(float f, NvU32 n = 1)
{
    NvU32 &fi = (NvU32 &)f;
    NvU32 fiNext;
    if (fi & 0x80000000)
    {
        fiNext = fi - n;
    }
    else
    {
        fiNext = fi + n;
        if (!(fiNext & 0x7f800000))
            fiNext |= 0x800000;
    }
#if ASSERT_ONLY_CODE
    float &fNext = (float &)fiNext;
    nvAssert(isnormal(fNext) && fNext > f);
#endif
    return (float &)fiNext;
}
// make n steps to the left from f
inline float prev_float(float f, unsigned n = 1)
{
    float fPrev;
    if (f > 0)
    {
        (unsigned &)fPrev = (unsigned &)f - n;
    }
    else
    {
        (unsigned &)fPrev = (unsigned &)f + n;
    }
    nvAssert(isnormal(fPrev) && fPrev < f);
    return fPrev;
}

template <class T>
inline void MySafeRelease(T *(&p))
{
  if (p)
  {
    p->Release();
    p = NULL;
  }
}

template <class T>
inline void SAFE_DELETE_ONE(T *(&p))
{
  delete p;
  p = NULL;
}

inline NvU32 BIT(NvU32 index)
{
    nvAssert(index <= 31);
    return (1U << index);
}

template <class T>
inline void nvSwap(T &a, T&b)
{
    T tmp = a;
    a = b;
    b = tmp;
}

inline NvU64 sqr64(NvU32 a)
{
    return a * (NvU64)a;
}

template <class T>
inline T mySqr(const T f) { return f * f; }

inline double grad2Rad(const double grad)
{
  return grad / 180 * M_PI;
}

inline double rad2Grad(const double rad)
{
    return rad / M_PI * 180;
}

template <class T>
inline T mysign(T f)
{
    return f < 0 ? -1 : 1;
}

template <class T>
inline NvU32 binarySearch(const T *pValues, NvU32 nValues, const T &value)
{
	NvU32 minIndex = 0;
	nvAssert(nValues > 0);
	NvU32 maxIndex = nValues - 1;
	while (maxIndex > minIndex)
	{
		NvU32 middle = (maxIndex + minIndex) / 2;
		if (pValues[middle] <= value)
		{
			minIndex = middle;
		}
		else
		{
			maxIndex = middle;
		}
	}
	return minIndex;
}

#define mymin(a, b) ((a) < (b) ? a : b)
#define mymax(a, b) ((a) > (b) ? a : b)
#define myabs(a) ((a) >= 0 ? (a) : -(a))
#define myClamp(a, b, c) mymin(mymax(a, b), c)

inline bool aboutEqual(double f1, double f2)
{
    return fabs(f1 - f2) < 0.0001;
}

template <class T> static inline T pow3(T u) { return u * u * u; }
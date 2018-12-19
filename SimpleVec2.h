#pragma once

#include "MyMisc.h"

template <class T>
struct SimpleVec2
{
  inline SimpleVec2() { }
  inline SimpleVec2(double x, double y)
  {
     v[0] = x;
     v[1] = y;
  }
  inline const T &operator[](NvU32 u) const { nvAssert(u < 2); return v[u]; }
  inline T &operator[](NvU32 u) { nvAssert(u < 2); return v[u]; }
  inline SimpleVec2<T> operator /(const double c) const { return SimpleVec2<T>(v[0] / c, v[1] / c); }
  inline SimpleVec2<T> operator *(const double c) const { return SimpleVec2<T>(v[0] * c, v[1] * c); }
  template <class VECTOR>
  inline SimpleVec2<T> operator +=(const VECTOR &other) { nvAssert(other.size() == 2); v[0] = (T)(v[0] + other[0]); v[1] = (T)(v[1] + other[1]); return *this; }
  template <class VECTOR>
  inline SimpleVec2<T> operator +(const VECTOR &other) const { nvAssert(other.size() == 2); return SimpleVec2<T>(v[0] + other[0], v[1] + other[1]); }

  static inline NvU32 size() { return 2; }

private:
  T v[2];
};

typedef SimpleVec2<double> SimpleDVec2;
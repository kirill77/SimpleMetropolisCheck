#pragma once

template <class T, NvU32 N>
struct SimpleSafeArray
{
  inline SimpleSafeArray() { memset(this, 0, sizeof(*this)); }
  inline T &operator[](NvU32 u) { nvAssert(u < N); return m_data[u]; }
  inline const T &operator[](NvU32 u) const { nvAssert(u < N); return m_data[u]; }
  static inline NvU32 size() { return N; }
  inline T *rbegin() { return &m_data[N - 1]; }

private:
  T m_data[N];
};
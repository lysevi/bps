#pragma once

#include <cstdint>
#ifdef MSVC
#include <intrin.h>
//#define NOMINMAX
#include <windows.h>
#endif

namespace rstore::utils {
struct BitOperations {
  template <class T> static inline uint8_t get(T v, uint8_t num) {
    return (v >> num) & 1;
  }

  template <class T> static inline bool check(T v, uint8_t num) {
    return get(v, num) == 1;
  }

  template <class T> static inline T set(T v, uint8_t num) {
    return v | (static_cast<T>(T(1) << num));
  }

  template <class T> static inline T clr(T v, uint8_t num) { return v & ~(T(1) << num); }
};


#ifdef MSVC
uint8_t __inline ctz(uint64_t value) {
  DWORD trailing_zero = 0;

  if (_BitScanForward64(&trailing_zero, value)) {
    return static_cast<uint8_t>(trailing_zero);
  } else {
    return uint8_t(64);
  }
}

uint8_t __inline clz(uint64_t value) {
  DWORD leading_zero = 0;

  if (_BitScanReverse64(&leading_zero, value)) {
    return static_cast<uint8_t>(63 - leading_zero);
  } else {
    return uint8_t(64);
  }
}
#elif defined(GNU_CPP) || defined(CLANG_CPP)
inline uint8_t clz(uint64_t x) {
  return static_cast<uint8_t>(__builtin_clzll(x));
}
inline uint8_t ctz(uint64_t x) {
  return static_cast<uint8_t>(__builtin_ctzll(x));
}
#else
static uint8_t inline clz(uint64_t v) {
  const int value_max_bit_pos = sizeof(v) * 8 - 1;
  uint8_t result = 0;
  for (int8_t i = value_max_bit_pos; i >= 0; i -= 2) {
    if (BitOperations::check(v, i)) {
      break;
    } else {
      result++;
    }

    if (BitOperations::check(v, i - 1)) {
      break;
    } else {
      result++;
    }
  }
  return result;
}
static uint8_t inline ctz(uint64_t v) {
  const int value_max_bit_pos = sizeof(v) * 8 - 1;
  uint8_t result = 0;
  for (int8_t i = 0; i < value_max_bit_pos; i += 2) {
    if (BitOperations::check(v, i)) {
      break;
    } else {
      result++;
    }

    if (BitOperations::check(v, i + 1)) {
      break;
    } else {
      result++;
    }
  }
  return result;
}

#endif
}


#pragma once
#include <rstore/exports.h>

#include <cstdint>

namespace rstore {
using slice_size_t = uint32_t;
struct Slice {
  slice_size_t size;
  uint8_t *data;

  Slice()
      : size(0)
      , data(nullptr) {}

  Slice(slice_size_t sz, uint8_t *val)
      : size(sz)
      , data(val) {}

  EXPORT Slice(Slice &&cp);
  EXPORT void operator=(Slice &&cp);
  EXPORT Slice(const Slice &cp);
  EXPORT void operator=(const Slice &cp);

  ~Slice() { free_memory(); }

  // <  0 if this <  o,
  // == 0 if this == o,
  // >  0 if this >  o
  EXPORT int compare(const Slice &o) const;
  void free_memory() {
    if (data != nullptr) {
      delete[] data;
      data = nullptr;
    }
    size = 0;
  }
};
} // namespace rstore
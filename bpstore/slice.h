#pragma once
#include <bpstore/exports.h>

#include <cstdint>

namespace bpstore {
using slice_size_t = uint32_t;
struct slice_t {
  slice_size_t size;
  uint8_t *data;

  slice_t()
      : size(0)
      , data(nullptr) {}

  slice_t(slice_size_t sz, uint8_t *val)
      : size(sz)
      , data(val) {}

  EXPORT slice_t(slice_t &&cp);
  EXPORT void operator=(slice_t &&cp);
  EXPORT slice_t(const slice_t &cp);
  EXPORT void operator=(const slice_t &cp);

  ~slice_t() {
    if (data != nullptr) {
      delete[] data;
      data = nullptr;
    }
    size = 0;
  }

  // <  0 if this <  o,
  // == 0 if this == o,
  // >  0 if this >  o
  EXPORT int compare(const slice_t &o) const;
  void free_memory() {
    delete[] data;
    data = nullptr;
    size = 0;
  }
};
}
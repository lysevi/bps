#pragma once
#include <rstore/exports.h>

#include <cstdint>
#include <string>

namespace rstore {
using slice_size_t = uint32_t;
struct Slice {
  slice_size_t size;
  const char *data;

  Slice()
      : size(0)
      , data(nullptr) {}

  Slice(slice_size_t sz, const char *val)
      : size(sz)
      , data(val) {}

  Slice(slice_size_t sz, const std::string &val)
      : size(sz)
      , data(val.data()) {}

  
  // <  0 if this <  o,
  // == 0 if this == o,
  // >  0 if this >  o
  EXPORT int compare(const Slice &o) const;
};
} // namespace rstore
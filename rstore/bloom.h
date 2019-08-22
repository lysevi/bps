#pragma once

#include <rstore/exports.h>

#include <cstdint>
#include <functional>
#include <array>


namespace rstore {
struct Bloom {

  EXPORT Bloom(bool *fltr_, size_t size_);
  EXPORT void add(uint8_t *data, size_t data_size);
  EXPORT bool find(uint8_t *data, size_t data_size) const;

  std::array<std::function<uint64_t(uint8_t *, size_t)>, 2> functions;
  bool *fltr;
  size_t s;
};
} // namespace rstore
#pragma once

#include <rstore/exports.h>

#include <cstdint>
#include <functional>
#include <array>
#include <vector>

namespace rstore::inner {
struct Bloom {

  EXPORT Bloom(std::vector<bool>&fltr_);
  EXPORT void add(uint8_t *data, size_t data_size);
  EXPORT bool find(uint8_t *data, size_t data_size) const;

  std::array<std::function<uint64_t(uint8_t *, size_t)>, 2> functions;
  std::vector<bool> fltr;
};
} // namespace rstore
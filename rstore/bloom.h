#pragma once

#include <rstore/exports.h>

#include <array>
#include <cstdint>
#include <functional>
#include <vector>

namespace rstore::inner {

struct Bloom {
  EXPORT static void add(std::vector<bool> &fltr, uint8_t *data, size_t data_size);
  EXPORT static bool find(const std::vector<bool> &fltr, uint8_t *data, size_t data_size);
};
} // namespace rstore::inner
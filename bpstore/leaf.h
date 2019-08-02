#pragma once
#include <bpstore/exports.h>
#include <bpstore/slice.h>

#include <cstdint>
#include <limits>
#include <optional>
#include <vector>

namespace bpstore {
using capacity_t = uint32_t;

struct leaf_t {
  capacity_t capacity;
  capacity_t size;

  std::vector<slice_t> keys;
  std::vector<slice_t> values;

  EXPORT leaf_t(capacity_t cap);
  EXPORT ~leaf_t();

  EXPORT bool insert(slice_t &k, slice_t &v);
  EXPORT std::optional<slice_t> find(const slice_t &k) const;
};
} // namespace bpstore
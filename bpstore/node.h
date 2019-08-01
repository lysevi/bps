#pragma once
#include <bpstore/exports.h>
#include <bpstore/slice.h>

#include <cstdint>
#include <optional>
#include <vector>
#include <limits>

namespace bpstore {
using storage_ptr_t = uint64_t;
const storage_ptr_t k_storage_ptr_null = std::numeric_limits<storage_ptr_t>::max();

struct leaf_t {
  bool is_leaf;
  uint32_t capacity;
  uint32_t size;

  std::vector<slice_t> keys;
  std::vector<slice_t> values;
  //std::vector<storage_ptr_t> children;

  EXPORT leaf_t(bool is_leaf_, uint32_t cap);
  EXPORT ~leaf_t();

  EXPORT bool insert(slice_t &k, slice_t &v);
  EXPORT std::optional<slice_t> find(const slice_t &k) const;
};

} // namespace bpstore
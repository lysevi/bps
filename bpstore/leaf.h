#pragma once
#include <bpstore/exports.h>
#include <bpstore/slice.h>

#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <vector>

namespace bpstore {
using capacity_t = uint32_t;
using storage_ptr_t = uint64_t;
const storage_ptr_t k_storage_ptr_null = std::numeric_limits<storage_ptr_t>::max();

struct leaf_t {
  storage_ptr_t address = k_storage_ptr_null;

  capacity_t capacity;
  capacity_t size;

  std::vector<slice_t> keys;
  std::vector<slice_t> values;

  EXPORT leaf_t(capacity_t cap);
  EXPORT ~leaf_t();

  EXPORT bool insert(slice_t &k, slice_t &v) noexcept;
  EXPORT std::optional<slice_t> find(const slice_t &k) const noexcept;

  bool empty() const { return size == 0; }
  const slice_t *first_key() const noexcept {
    if (size == 0) {
      return nullptr;
    } else {
      return &keys[0];
    }
  }

  const slice_t *last_key() const noexcept {
    if (size == 0) {
      return nullptr;
    } else {
      return &keys[size - 1];
    }
  }
};

using leaf_ptr_t = std::shared_ptr<leaf_t>;
} // namespace bpstore
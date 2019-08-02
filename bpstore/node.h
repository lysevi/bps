#pragma once
#include <bpstore/exports.h>
#include <bpstore/slice.h>

#include <cstdint>
#include <limits>
#include <optional>
#include <vector>

namespace bpstore {
using storage_ptr_t = uint64_t;
using capacity_t = uint32_t;
const storage_ptr_t k_storage_ptr_null = std::numeric_limits<storage_ptr_t>::max();

template <class T>
void insert_to_array(T *array, size_t sz, size_t insert_pos, const T &value) {
  for (auto i = sz - 1; i > insert_pos; i--) {
    std::swap(array[i], array[i - 1]);
  }
  array[insert_pos] = value;
}

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

struct node_t {
  bool children_is_leaf = false;
  uint32_t capacity;
  uint32_t size;
  std::vector<slice_t> keys;
  std::vector<storage_ptr_t> children;

  EXPORT node_t(uint32_t cap);
  EXPORT ~node_t();
};

} // namespace bpstore
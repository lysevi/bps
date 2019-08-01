#pragma once
#include <bpstore/exports.h>
#include <bpstore/slice.h>

#include <cstdint>
#include <optional>
#include <vector>

namespace bpstore {
using storage_ptr_t = uint64_t;

struct node_t {
  bool is_leaf;
  uint32_t capacity;
  uint32_t size;

  std::vector<slice_t> keys;
  std::vector<slice_t> values;
  std::vector<storage_ptr_t> children;

  EXPORT node_t(bool is_leaf_, uint32_t cap);
  EXPORT ~node_t();

  EXPORT bool insert(slice_t &k, slice_t &v);
  EXPORT std::optional<slice_t> find(const slice_t &k) const;
};

} // namespace bpstore
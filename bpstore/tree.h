#pragma once

#include <bpstore/exports.h>
#include <bpstore/leaf.h>
#include <bpstore/utils/logger.h>

namespace bpstore {

struct iblock_storage {};

struct tree_params_t {
  capacity_t node_capacity;
};

using storage_ptr_t = uint64_t;
const storage_ptr_t k_storage_ptr_null = std::numeric_limits<storage_ptr_t>::max();

struct node_t {
  bool children_is_leaf = false;
  uint32_t capacity;
  uint32_t size;
  std::vector<slice_t> keys;
  std::vector<storage_ptr_t> children;

  EXPORT node_t(uint32_t cap);
  EXPORT ~node_t();
};

class tree_t {
public:
  EXPORT tree_t(const bpstore::utils::logging::abstract_logger_ptr &logger, iblock_storage *storage);
  EXPORT ~tree_t();

  EXPORT bool insert(slice_t &k, slice_t &v);
  EXPORT std::optional<slice_t> find(const slice_t &k) const;

private:
  iblock_storage *const _storage;
  bpstore::utils::logging::abstract_logger_ptr _logger;
};

} // namespace bpstore
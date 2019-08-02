#pragma once

#include <bpstore/exports.h>
#include <bpstore/leaf.h>
#include <bpstore/utils/logger.h>

#include <memory>

namespace bpstore {

struct tree_params_t {
  capacity_t node_capacity;
};

struct node_t {
  storage_ptr_t address = k_storage_ptr_null;
  bool children_is_leaf = false;
  uint32_t capacity;
  uint32_t size;
  std::vector<slice_t> keys;
  std::vector<storage_ptr_t> children;

  EXPORT node_t(uint32_t cap);
  EXPORT ~node_t();
};

using node_ptr_t = std::shared_ptr<node_t>;

struct iblock_storage {
  virtual leaf_ptr_t load_max_leaf(tree_params_t &params) = 0;
  virtual leaf_ptr_t create_leaf(tree_params_t &params) = 0;
  virtual leaf_ptr_t load_leaf(const storage_ptr_t ptr) = 0;

  virtual node_ptr_t load_node(const storage_ptr_t ptr) = 0;
  virtual node_ptr_t create_node(tree_params_t &params) = 0;
};

class tree_t {
public:
  EXPORT
  tree_t(const tree_params_t &params,
         const bpstore::utils::logging::abstract_logger_ptr &logger,
         iblock_storage *storage);
  EXPORT ~tree_t();

  EXPORT bool insert(slice_t &k, slice_t &v);
  EXPORT std::optional<slice_t> find(const slice_t &k) const;

private:
  tree_params_t _params;
  iblock_storage *const _storage;
  bpstore::utils::logging::abstract_logger_ptr _logger;

  leaf_ptr_t _last_leaf;
};

} // namespace bpstore
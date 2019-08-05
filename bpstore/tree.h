#pragma once

#include <bpstore/exports.h>
#include <bpstore/leaf.h>
#include <bpstore/utils/logger.h>

#include <memory>
#include <utility>
#include <vector>

namespace bpstore {

struct tree_params_t {
  capacity_t node_capacity;
  float node_filling_percent = 1.0;
};

struct node_t {
  bool is_root = false;
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
  using save_result_t = std::pair<std::vector<leaf_ptr_t>, std::vector<node_ptr_t>>;
  virtual leaf_ptr_t load_max_leaf(tree_params_t &params) = 0;
  virtual leaf_ptr_t create_leaf(tree_params_t &params) = 0;
  virtual leaf_ptr_t load_leaf(const storage_ptr_t ptr) = 0;

  virtual node_ptr_t load_root() = 0;
  virtual node_ptr_t load_node(const storage_ptr_t ptr) = 0;
  virtual node_ptr_t create_node(tree_params_t &params) = 0;

  virtual save_result_t save(const std::vector<leaf_ptr_t> &leafs,
                             const std::vector<node_ptr_t> &nodes)
      = 0;
};

class tree_t {
public:
  EXPORT
  tree_t(const tree_params_t &params,
         const bpstore::utils::logging::abstract_logger_ptr &logger,
         iblock_storage *storage);
  EXPORT ~tree_t();

  EXPORT bool insert(const slice_t &k, const slice_t &v);
  EXPORT std::optional<slice_t> find(const slice_t &k) const;

protected:
  leaf_ptr_t target(const slice_t &k) const;

private:
  iblock_storage *const _storage;
  bpstore::utils::logging::abstract_logger_ptr _logger;
  tree_params_t _params;

  leaf_ptr_t _last_leaf;
  node_ptr_t _root;
};

} // namespace bpstore
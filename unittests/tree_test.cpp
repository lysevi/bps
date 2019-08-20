#include <rstore/slice_helpers.h>
#include <rstore/tree.h>

#include "helpers.h"
#include <algorithm>
#include <random>
#include <utility>
#include <vector>

#include <catch.hpp>

#include <list>
#include <map>

using rstore::utils::logging::logger_manager;
using rstore::utils::logging::prefix_logger;
using namespace rstore;

struct mock_block_storage final : iblock_storage {
  virtual ~mock_block_storage() {}

  leaf_ptr_t load_max_leaf(tree_params_t &) override {
    if (leafs.empty()) {
      return nullptr;
    }
    return leafs.rbegin()->second;
  }

  leaf_ptr_t create_leaf(tree_params_t &params) override {
    auto res = std::make_shared<leaf_t>(params.node_capacity);
    res->address = next_addr++;
    leafs[res->address] = res;
    return res;
  }

  leaf_ptr_t load_leaf(const storage_ptr_t ptr) override {
    if (auto it = leafs.find(ptr); it != leafs.end()) {
      return it->second;
    } else {
      return nullptr;
    }
  }

  node_ptr_t load_root() override {
    std::list<node_ptr_t> roots;
    for (auto &kv : nodes) {
      if (kv.second->is_root) {
        roots.push_back(kv.second);
      }
    }
    auto max_pos = std::max_element(
        roots.cbegin(), roots.cend(), [](const auto &n1, const auto &n2) {
          return n1->address < n2->address;
        });
    if (max_pos == roots.end()) {
      return nullptr;
    }
    return *max_pos;
  }

  node_ptr_t load_node(const storage_ptr_t ptr) override {
    if (auto it = nodes.find(ptr); it != nodes.end()) {
      return it->second;
    } else {
      return nullptr;
    }
  }

  node_ptr_t create_node(tree_params_t &params) override {
    auto res = std::make_shared<node_t>(params.node_capacity);
    res->address = next_addr++;
    nodes[res->address] = res;
    return res;
  }

  iblock_storage::save_result_t save(const std::vector<leaf_ptr_t> &ls,
                                     const std::vector<node_ptr_t> &ns) override {
    std::vector<leaf_ptr_t> lsaved(ls.size());
    std::vector<node_ptr_t> nsaved(ns.size());
    for (auto &lptr : ls) {
      auto new_l = std::make_shared<leaf_t>(*lptr);
      new_l->address = ++next_addr;
      leafs[lptr->address] = new_l;
    }

    for (auto &lptr : ns) {
      auto new_l = std::make_shared<node_t>(*lptr);
      new_l->address = ++next_addr;
      nodes[lptr->address] = new_l;
    }
    return {lsaved, nsaved};
  }

  std::map<storage_ptr_t, leaf_ptr_t> leafs;
  std::map<storage_ptr_t, node_ptr_t> nodes;
  storage_ptr_t next_addr = 0;
};

TEMPLATE_TEST_CASE("storage.tree_t: insert/find kv",
                   "[store]",
                   (std::pair<size_t, uint8_t>)) {
  auto tst_logger = std::make_shared<prefix_logger>(
      logger_manager::instance()->get_shared_logger(), "test?> ");
  auto tree_logger = std::make_shared<rstore::utils::logging::prefix_logger>(
      logger_manager::instance()->get_shared_logger(), "tree: ");

  tree_params_t params;
  params.node_capacity = 3;
  auto storage = std::make_unique<mock_block_storage>();

  EXPECT_TRUE(storage->leafs.empty());
  tree_t t(params, tree_logger, storage.get());
  EXPECT_EQ(storage->leafs.size(), 1);

  using test_key_type = typename TestType::first_type;
  using test_val_type = typename TestType::second_type;

  std::vector<test_key_type> keys(params.node_capacity);

  // SECTION("storage.tree_t(0..cap)") {
  std::iota(keys.begin(), keys.end(), test_key_type(0));
  //}

  // SECTION("storage.tree_t(cap..0)") {
  //  std::iota(keys.rbegin(), keys.rend(), test_key_type(0));
  //}

  test_val_type tval(0);
  for (auto k : keys) {
    auto slice_k = slice_make_from(k);
    auto slice_v = slice_make_from(tval);
    tval++;

    auto res = t.insert(slice_k, slice_v);
    if (!res) {
      EXPECT_TRUE(res);
    }
    auto founded = t.find(slice_k);
    EXPECT_TRUE(founded.has_value());
    EXPECT_EQ(founded.value().compare(slice_v), 0);
  }
}

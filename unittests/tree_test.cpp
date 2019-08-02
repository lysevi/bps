#include <bpstore/slice_helpers.h>
#include <bpstore/tree.h>

#include "helpers.h"
#include <algorithm>
#include <random>
#include <utility>
#include <vector>

#include <catch.hpp>

#include <map>

using bpstore::utils::logging::logger_manager;
using bpstore::utils::logging::prefix_logger;
using namespace bpstore;

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

  std::map<storage_ptr_t, leaf_ptr_t> leafs;
  std::map<storage_ptr_t, node_ptr_t> nodes;
  storage_ptr_t next_addr = 0;
};

TEMPLATE_TEST_CASE("storage.tree_t: insert/find kv",
                   "[store]",
                   (std::pair<size_t, uint8_t>)) {
  auto tst_logger = std::make_shared<prefix_logger>(
      logger_manager::instance()->get_shared_logger(), "test?> ");
  auto tree_logger = std::make_shared<bpstore::utils::logging::prefix_logger>(
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

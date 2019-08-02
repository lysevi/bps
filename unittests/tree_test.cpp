#include <bpstore/slice_helpers.h>
#include <bpstore/tree.h>

#include "helpers.h"
#include <algorithm>
#include <random>
#include <utility>
#include <vector>

#include <catch.hpp>

using bpstore::utils::logging::logger_manager;
using bpstore::utils::logging::prefix_logger;
using namespace bpstore;

struct mock_block_storage : iblock_storage {};

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

  tree_t t(tree_logger, storage.get());

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
    EXPECT_TRUE(res);

    auto founded = t.find(slice_k);
    EXPECT_TRUE(founded.has_value());
    EXPECT_EQ(founded.value().compare(slice_v), 0);
  }
}

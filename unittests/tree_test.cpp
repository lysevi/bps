#include <bpstore/slice_helpers.h>
#include <bpstore/tree.h>

#include "helpers.h"
#include <algorithm>
#include <random>
#include <utility>
#include <vector>

#include <catch.hpp>

TEMPLATE_TEST_CASE("storage.tree_t: insert/find kv",
                   "[store]",
                   (std::pair<size_t, uint8_t>)) {
  using namespace bpstore;
  using test_key_type = typename TestType::first_type;
  using test_val_type = typename TestType::second_type;

  tree_params_t params;
  params.node_capacity = 3;
  
  //std::vector<test_key_type> keys(params.node_capacity);

  // SECTION("storage.tree_t(0..cap)") {
  //  std::iota(keys.begin(), keys.end(), test_key_type(0));
  //}

  // SECTION("storage.tree_t(cap..0)") {
  //  std::iota(keys.rbegin(), keys.rend(), test_key_type(0));
  //}

  /*test_val_type tval(0);
  for (auto k : keys) {
    tval++;
    auto slice_k = slice_make_from(k);
    auto slice_v = slice_make_from(tval);
  }*/
}

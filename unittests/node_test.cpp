#include <bpstore/node.h>
#include <bpstore/slice_helpers.h>

#include "helpers.h"
#include <algorithm>
#include <random>
#include <utility>
#include <vector>

#include <catch.hpp>

TEMPLATE_TEST_CASE("storage.node_t: insert/find kv",
                   "[store]",
                   (std::pair<size_t, uint8_t>)) {
  using namespace bpstore;
  using test_key_type = typename TestType::first_type;
  using test_val_type = typename TestType::second_type;

  uint32_t node_cap = 10;
  std::vector<test_key_type> keys(node_cap);
  bool write_twice = false;
  bool shuffled_keys = false;

  SECTION("storage.node_t(0..cap)") {
    SECTION("storage.node_t(0..cap). write twice") { write_twice = true; }
    SECTION("storage.node_t(0..cap). write once") { write_twice = false; }
    SECTION("storage.node_t(0..cap). shuffled") { shuffled_keys = true; }
    std::iota(keys.begin(), keys.end(), test_key_type(0));
  }

  SECTION("storage.node_t(cap..0)") {
    SECTION("storage.node_t(cap..0). write twice") { write_twice = true; }
    SECTION("storage.node_t(cap..0). write once") { write_twice = false; }
    SECTION("storage.node_t(cap..0). shuffled") { shuffled_keys = true; }
    std::iota(keys.rbegin(), keys.rend(), test_key_type(0));
  }

  node_t n(true, node_cap);
  test_val_type tval(0);
  if (shuffled_keys) {
    std::shuffle(keys.begin(), keys.end(), std::mt19937{std::random_device{}()});
  }
  for (auto k : keys) {
    tval++;
    auto slice_k = slice_make_from(k);
    auto slice_v = slice_make_from(tval);
    auto res = n.insert(slice_k, slice_v);

    auto is_last = (keys.front() < keys.back() ? k == node_cap : k == 0);
    EXPECT_TRUE(res || is_last);
    if (res) {
      auto v = n.find(slice_k);
      EXPECT_TRUE(v.has_value());
      EXPECT_TRUE(v.value().compare(slice_v) == 0);
    }
    if (write_twice) {
      tval++;
      slice_v = slice_make_from(tval);
      res = n.insert(slice_k, slice_v);

      EXPECT_TRUE(res);

      if (res) {
        auto v = n.find(slice_k);
        EXPECT_TRUE(v.has_value());
        EXPECT_TRUE(v.value().compare(slice_v) == 0);
      }
    }
    if (n.size > 1) {
      for (size_t i = 1; i < n.size; ++i) {
        int cmp = n.keys[i].compare(n.keys[i - 1]);
        EXPECT_GT(cmp, 0);
      }
    }
  }
}

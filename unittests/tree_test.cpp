#include "helpers.h"
#include <catch.hpp>
#include <rstore/slice_helpers.h>
#include <rstore/tree.h>

TEST_CASE("tree.memlevel", "[store]") {
  size_t B = 0;
  SECTION("tree.memlevel.B=2") { B = size_t(2); }
  SECTION("tree.memlevel.B=10") { B = size_t(10); }
  SECTION("tree.memlevel.B=100") { B = size_t(100); }

  auto lvl = std::make_shared<rstore::inner::MemLevel>(B);
  EXPECT_EQ(lvl->_cap, B);
  EXPECT_EQ(lvl->_size, size_t(0));
  EXPECT_EQ(lvl->_kv.size(), B);

  size_t k = 1000;
  size_t v = 1000;
  while (lvl->insert(rstore::slice_make_from(k), rstore::slice_make_from(v))) {
    auto answer = lvl->find(rstore::slice_make_from(k));
    EXPECT_TRUE(answer.has_value());
    const auto unpacked = rstore::slice_convert_to<size_t>(answer.value());
    EXPECT_EQ(unpacked, v);
    --k;
    v += size_t(2);
  }

  lvl->sort();
  auto k1 = lvl->_kv.front().first;
  auto k2 = lvl->_kv.back().first;

  auto k1_v = rstore::slice_convert_to<size_t>(lvl->find(k1).value());
  auto k2_v = rstore::slice_convert_to<size_t>(lvl->find(k2).value());

  EXPECT_LT(k1.compare(k2), 0);
  EXPECT_GT(k1_v, k2_v);
}

TEST_CASE("tree", "[store]") {
  rstore::Tree::Params params;
  SECTION("tree.params.B=2") { params.B = size_t(2); }
  SECTION("tree.params.B=10") { params.B = size_t(10); }
  SECTION("tree.params.B=100") { params.B = size_t(100); }

  rstore::Tree t(params);
  t.init();

  size_t k = 0;
  size_t v = 0;
  for (size_t i = 0; i < params.B; ++i) {
    t.insert(rstore::slice_make_from(k), rstore::slice_make_from(v));

    auto answer = t.find(rstore::slice_make_from(k));
    EXPECT_TRUE(answer.has_value());
    const auto unpacked = rstore::slice_convert_to<size_t>(answer.value());
    EXPECT_EQ(unpacked, v);
    ++k;
    v += size_t(2);
  }

  t.insert(rstore::slice_make_from(k), rstore::slice_make_from(v));
}

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
  EXPECT_EQ(lvl->_keys.size(), B);
  EXPECT_EQ(lvl->_vals.size(), B);

  size_t k = 0;
  size_t v = 0;
  while (lvl->insert(rstore::slice_make_from(k), rstore::slice_make_from(v))) {
    auto answer = lvl->find(rstore::slice_make_from(k));
    EXPECT_TRUE(answer.has_value());
    const auto unpacked = rstore::slice_convert_to<size_t>(answer.value());
    EXPECT_EQ(unpacked, v);
    ++k;
    v += size_t(2);
  }
}

TEST_CASE("tree", "[store]") {
  rstore::Tree::Params params;
  SECTION("tree.params.B=2") { params.B = size_t(2); }
  SECTION("tree.params.B=10") { params.B = size_t(10); }
  SECTION("tree.params.B=100") { params.B = size_t(100); }

  rstore::Tree t(params);
  t.init();
}

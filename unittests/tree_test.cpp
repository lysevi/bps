#include "helpers.h"
#include <catch.hpp>
#include <iostream>
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
  auto k1 = lvl->_keys.front();
  auto k2 = lvl->_keys.back();

  auto k1_v = rstore::slice_convert_to<size_t>(lvl->find(k1).value());
  auto k2_v = rstore::slice_convert_to<size_t>(lvl->find(k2).value());

  EXPECT_LT(k1.compare(k2), 0);
  EXPECT_GT(k1_v, k2_v);
}

TEST_CASE("tree.kmerge", "[store]") {
  size_t count = 3;
  std::vector<rstore::inner::INode *> src;
  src.reserve(count * 2);
  size_t k = 0;
  for (size_t i = 0; i < count; ++i) {
    auto cur_size = 3;
    auto mem_l = new rstore::inner::MemLevel(cur_size);
    auto low_l = new rstore::inner::LowLevel(cur_size);

    for (size_t j = 0; j < cur_size; ++j) {
      mem_l->insert(rstore::slice_make_from(k), rstore::slice_make_from(k * 10));
      k++;
      low_l->insert(rstore::slice_make_from(k), rstore::slice_make_from(k * 10));
      k++;
    }
    mem_l->sort();
    src.push_back(mem_l);
    src.push_back(low_l);
  }

  auto total_size = std::accumulate(
      src.cbegin(), src.cend(), size_t(0), [](size_t r, const rstore::inner::INode *n) {
        return r + n->size();
      });

  auto dest = std::make_shared<rstore::inner::LowLevel>(total_size);

  rstore::inner::kmerge(dest.get(), src);

  for (auto it = dest->_keys.begin(); it != dest->_keys.end(); ++it) {
    auto k1 = rstore::slice_convert_to<size_t>(*it);
    auto next = std::next(it);
    if (next == dest->_keys.end()) {
      break;
    }
    for (auto subit = next; subit != dest->_keys.end(); ++subit) {
      auto k2 = rstore::slice_convert_to<size_t>(*subit);
      EXPECT_LE(k1, k2);
    }
  }

  for (size_t target_key = 0; target_key < k; ++target_key) {
    auto answer = dest->find(rstore::slice_make_from(target_key));
    EXPECT_TRUE(answer.has_value());
    auto value = rstore::slice_convert_to<size_t>(answer.value());
    EXPECT_EQ(target_key * 10, value);
  }

  for (auto v : src) {
    delete v;
  }
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
  while (t.deep() < 7) {
    t.insert(rstore::slice_make_from(k), rstore::slice_make_from(v));
    auto answer = t.find(rstore::slice_make_from(k));
    EXPECT_TRUE(answer.has_value());
    const auto unpacked = rstore::slice_convert_to<size_t>(answer.value());
    EXPECT_EQ(unpacked, v);
    ++k;
    v += size_t(2);
  }
}

#include <rstore/bloom.h>

#include "helpers.h"
#include <algorithm>
#include <catch.hpp>

TEST_CASE("bloom", "[store]") {
  size_t data_size = 0;
  SECTION("bloom.10") { data_size = 10; }
  SECTION("bloom.50") { data_size = 50; }
  SECTION("bloom.100") { data_size = 100; }

  std::vector<bool> fltr(data_size);
  std::fill(fltr.begin(), fltr.end(), false);

  std::vector<uint8_t> data(data_size);

  for (size_t i = 0; i < data_size; ++i) {
    data[i] = 1;
    rstore::inner::Bloom::add(fltr, data.data(), data.size());
    EXPECT_TRUE(rstore::inner::Bloom::find(fltr, data.data(), data.size()));
  }
}

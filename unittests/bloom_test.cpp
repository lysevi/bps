#include <rstore/bloom.h>

#include "helpers.h"
#include <catch.hpp>
#include <cstring>

TEST_CASE("bloom", "[store]") {
  size_t data_size = 0;
  SECTION("bloom.10") { data_size = 10; }
  SECTION("bloom.50") { data_size = 50; }
  SECTION("bloom.100") { data_size = 100; }

  bool *fltr = new bool[data_size];
  std::memset(fltr, 0, sizeof(bool) * data_size);

  std::vector<uint8_t> data(data_size);

  rstore::Bloom blm(fltr, data_size);
  for (size_t i = 0; i < data_size; ++i) {
    data[i] = 1;
    blm.add(data.data(), data.size());
    EXPECT_TRUE(blm.find(data.data(), data.size()));
  }
  delete[] fltr;
}

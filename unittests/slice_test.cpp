#include <bpstore/slice.h>
#include <bpstore/slice_helpers.h>

#include "helpers.h"
#include <catch.hpp>

TEMPLATE_TEST_CASE("storage.slice_t", "[store]", size_t, uint32_t, uint64_t) {
  using namespace bpstore;
  for (TestType i = 1; i < 10; ++i) {
    auto slc = slice_make_from(i, sizeof(i));
    EXPECT_EQ(static_cast<TestType>(slc.size), sizeof(i));
    EXPECT_TRUE(
        std::any_of(slc.data, slc.data + slc.size, [](auto v) { return v != 0; }));

    auto converted = slice_convert_to<TestType>(slc);
    EXPECT_EQ(i, converted);

    auto slc2 = slice_make_from(i + 1);
    EXPECT_EQ(slc2.size, slc.size);
    EXPECT_EQ(slc.compare(slc2), -1);
    EXPECT_EQ(slc.compare(slc), 0);
    EXPECT_EQ(slc2.compare(slc), 1);
  }
}


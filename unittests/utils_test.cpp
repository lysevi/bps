#include <bpstore/utils/strings.h>
#include <bpstore/utils/utils.h>

#include "helpers.h"
#include <array>
#include <catch.hpp>
#include <numeric>


TEST_CASE("utils.split") {
  std::array<int, 8> tst_a;
  std::iota(tst_a.begin(), tst_a.end(), 1);

  std::string str = "1 2 3 4 5 6 7 8";
  auto splitted_s = bpstore::utils::strings::tokens(str);

  std::vector<int> splitted(splitted_s.size());
  std::transform(splitted_s.begin(),
                 splitted_s.end(),
                 splitted.begin(),
                 [](const std::string &s) { return std::stoi(s); });

  EXPECT_EQ(splitted.size(), size_t(8));

  bool is_equal
      = std::equal(tst_a.begin(), tst_a.end(), splitted.begin(), splitted.end());
  EXPECT_TRUE(is_equal);
}

TEST_CASE("utils.to_upper") {
  auto s = "lower string";
  auto res = bpstore::utils::strings::to_upper(s);
  EXPECT_EQ(res, "LOWER STRING");
}

TEST_CASE("utils.to_lower") {
  auto s = "UPPER STRING";
  auto res = bpstore::utils::strings::to_lower(s);
  EXPECT_EQ(res, "upper string");
}


void f_throw() {
  throw bpstore::utils::exceptions::exception_t("error");
}

TEST_CASE("utils.exception") {
  try {
    f_throw();
  } catch (bpstore::utils::exceptions::exception_t &e) {
    bpstore::utils::logging::logger_info(e.what());
  }
}

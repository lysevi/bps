#include <catch.hpp>
#include <numeric>
#include <rstore/utils/cz.h>
#include <rstore/utils/strings.h>
#include <rstore/utils/utils.h>

#ifdef rstore_ENABLE_BENCHMARKS
TEST_CASE("utils::strings", "[bench]") {
  const size_t data_size = 1024 * 1024;
  std::vector<uint8_t> data(data_size);
  std::iota(data.begin(), data.end(), uint8_t(0));

  BENCHMARK("strings::to_string small") {
    UNUSED(rstore::utils::strings::to_string("Hello, world!", int(1), float(3.14)));
  };

  BENCHMARK("strings::to_string") {
    UNUSED(rstore::utils::strings::to_string("Hello, world!",
                                             int(1),
                                             float(3.14),
                                             "Hello, World! Hello, world! Hello, World! "
                                             "Hello, world! Hello, Worl! Hello, world!",
                                             int(1),
                                             float(3.14),
                                             "Hello, World!"
                                             "Hello, world!",
                                             int(1),
                                             float(3.14),
                                             "Hello, World!"));
  };

  std::string target("H e l l o , w o r l d !", 10);

  BENCHMARK("strings::split") { UNUSED(rstore::utils::strings::split(target, ' ')); };
}

TEST_CASE("utils::cz", "[bench]") {

  BENCHMARK("utils::cz::clz") { UNUSED(rstore::utils::clz(3458764513820540928)); };
  BENCHMARK("utils::cz::ctz") { UNUSED(rstore::utils::ctz(240)); };
}
#endif
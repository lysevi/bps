#include <catch.hpp>
#include <numeric>
#include <bpstore/utils/strings.h>
#include <bpstore/utils/utils.h>

#ifdef BPSTORE_ENABLE_BENCHMARKS
TEST_CASE("utils::strings", "[bench]") {
  const size_t data_size = 1024 * 1024;
  std::vector<uint8_t> data(data_size);
  std::iota(data.begin(), data.end(), uint8_t(0));

  BENCHMARK("strings::to_string small") {
    UNUSED(bpstore::utils::strings::to_string("Hello, world!", int(1), float(3.14)));
  };

  BENCHMARK("strings::to_string") {
    UNUSED(bpstore::utils::strings::to_string("Hello, world!",
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

  BENCHMARK("strings::split") { UNUSED(bpstore::utils::strings::split(target, ' ')); };
}
#endif
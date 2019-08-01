#pragma once

#include <ctime>
#include <thread>

#include <bpstore/utils/exception.h>

#define UNUSED(x) (void)(x)

namespace bpstore::utils {

inline void sleep_mls(long long a) {
  std::this_thread::sleep_for(std::chrono::milliseconds(a));
}

struct elapsed_time {
  elapsed_time() noexcept { start_time = std::clock(); }
  [[nodiscard]] double elapsed() noexcept {
    return ((double)std::clock() - start_time) / CLOCKS_PER_SEC;
  }
  std::clock_t start_time;
};

} // namespace bpstore::utils

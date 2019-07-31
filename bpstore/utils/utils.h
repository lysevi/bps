#pragma once

#include <atomic>
#include <ctime>
#include <thread>

#include <bpstore/utils/exception.h>

#define UNUSED(x) (void)(x)

namespace bpstore::utils {

inline void sleep_mls(long long a) {
  std::this_thread::sleep_for(std::chrono::milliseconds(a));
}

class non_copy {
private:
  non_copy(const non_copy &) = delete;
  non_copy &operator=(const non_copy &) = delete;

protected:
  non_copy() = default;
};

struct elapsed_time {
  elapsed_time() noexcept { start_time = std::clock(); }
  [[nodiscard]] double elapsed() noexcept {
    return ((double)std::clock() - start_time) / CLOCKS_PER_SEC;
  }
  std::clock_t start_time;
};

} // namespace bpstore::utils

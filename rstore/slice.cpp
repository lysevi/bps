#include <algorithm>
#include <cstring>
#include <rstore/slice.h>
#include <rstore/utils/exception.h>

using namespace rstore;

int Slice::compare(const Slice &o) const {
  ENSURE(data != nullptr);
  ENSURE(o.data != nullptr);
  const uint32_t minimal_size = std::min(size, o.size);
  int result = std::memcmp(data, o.data, minimal_size);
  if (result == 0) {
    if (size < o.size) {
      return -1;
    } else if (size > o.size) {
      return 1;
    }
  }
  return result;
}

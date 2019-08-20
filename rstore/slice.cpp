#include <algorithm>
#include <cstring>
#include <rstore/slice.h>
#include <rstore/utils/exception.h>

using namespace rstore;

slice_t::slice_t(const slice_t &cp) {
  size = cp.size;
  if (cp.size > 0) {
    data = new uint8_t[size];
    std::memcpy(data, cp.data, size);
  }
}

void slice_t::operator=(const slice_t &cp) {
  if (data != nullptr && size == cp.size) {
    std::memcpy(data, cp.data, size);
    return;
  }

  if (data != nullptr) {
    free_memory();
  }
  if (cp.size > 0) {
    ENSURE(data == nullptr);
    ENSURE(size == 0);
    data = new uint8_t[cp.size];
    size = cp.size;
    std::memcpy(data, cp.data, size);
  }
}

slice_t::slice_t(slice_t &&cp)
    : size(cp.size)
    , data(std::move(cp.data)) {
  cp.size = 0;
  cp.data = nullptr;
}

void slice_t::operator=(slice_t &&cp) {
  std::swap(cp.data, data);
  std::swap(cp.size, size);
}

int slice_t::compare(const slice_t &o) const {
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

#pragma once

#include <rstore/slice.h>
#include <rstore/utils/exception.h>
#include <cstring>
#include <type_traits>

namespace rstore {

template <typename T>
T slice_convert_to(const slice_t &sl) {
  static_assert(std::is_pod_v<T>);
  ENSURE(sizeof(T) == sl.size);
  T result;
  std::memcpy(&result, sl.data, sl.size);
  return result;
}

template <typename T>
static slice_t slice_make_from(const T &v, const slice_size_t s) {
  static_assert(std::is_pod_v<T>);

  auto buf = new uint8_t[s];
  std::memcpy(buf, &v, s);

  return slice_t(s, buf);
}

template <typename T>
static std::enable_if_t<std::is_pod_v<T>,  slice_t> slice_make_from(const T &v) {
  auto s = sizeof(v);
  return slice_make_from(v, static_cast<slice_size_t>(s));
}
} // namespace rstore

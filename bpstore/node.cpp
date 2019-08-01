#include <algorithm>
#include <bpstore/node.h>
#include <bpstore/utils/utils.h>
#include <cstring>
#include <iterator>

using namespace bpstore;

slice_t::slice_t(const slice_t &cp) {
  size = cp.size;
  data = new uint8_t[cp.size];
  std::memcpy(data, cp.data, size);
}

void slice_t::operator=(const slice_t &cp) {
  size = cp.size;
  if (cp.size > 0) {
    data = new uint8_t[cp.size];
    std::memcpy(data, cp.data, size);
  }
}

template <class T>
void insert_to_array(T *array, size_t sz, size_t insert_pos, const T &value) {
  for (auto i = sz - 1; i > insert_pos; i--) {
    std::swap(array[i], array[i - 1]);
  }
  array[insert_pos] = value;
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

node_t::node_t(bool is_leaf_, uint32_t cap)
    : is_leaf(is_leaf_)
    , capacity(cap)
    , size(0)
    , keys(cap)
    , values(is_leaf ? cap : 0)
    , children(is_leaf ? 0 : cap) {}

node_t ::~node_t() {
  keys.clear();
  values.clear();
  children.clear();
}

bool node_t::insert(slice_t &k, slice_t &v) {
  ENSURE(is_leaf);
  ENSURE(keys.size() == capacity);
  ENSURE(values.size() == capacity);

  if (size == 0) {
    size++;
    keys[0] = k;
    values[0] = v;
    return true;
  } else {
    if (keys.front().compare(k) == 1) {
      if (size == capacity) {
        return false;
      }
      size++;
      insert_to_array(keys.data(), size, 0, k);
      insert_to_array(values.data(), size, 0, v);
      return true;
    }
  }

  auto lb_iter = std::lower_bound(
      this->keys.begin(),
      this->keys.begin() + size,
      k,
      [](const slice_t &l, const slice_t &r) { return l.compare(r) < 0; });

  if (lb_iter != this->keys.begin() + size) {
    auto d = std::distance(keys.begin(), lb_iter);
    if (lb_iter->compare(k) == 0) {
      values[d] = v;
      return true;
    }
    if (size == capacity) {
      return false;
    }
    size++;
    insert_to_array(keys.data(), size, d, k);
    insert_to_array(values.data(), size, d, v);
    return true;
  }
  if (size == capacity) {
    return false;
  }
  this->keys[size] = k;
  this->values[size] = v;
  size++;
  return true;
}

EXPORT std::optional<slice_t> node_t::find(const slice_t &k) const {
  auto lb_iter = std::lower_bound(
      this->keys.begin(),
      this->keys.begin() + size,
      k,
      [](const slice_t &l, const slice_t &r) { return l.compare(r) < 0; });

  auto ub_iter = std::upper_bound(
      this->keys.begin(),
      this->keys.begin() + size,
      k,
      [](const slice_t &l, const slice_t &r) { return l.compare(r) < 0; });

  if (lb_iter != keys.end()) {
    for (auto it = lb_iter; it != ub_iter; ++it) {
      if (it->compare(k) == 0) {
        auto result_position = std::distance(keys.begin(), it);
        return values[result_position];
      }
    }
  }
  return {};
}
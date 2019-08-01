#include <algorithm>
#include <bpstore/node.h>
#include <bpstore/utils/exception.h>
#include <bpstore/utils/utils.h>
#include <cstring>
#include <iterator>

using namespace bpstore;

template <class T>
void insert_to_array(T *array, size_t sz, size_t insert_pos, const T &value) {
  for (auto i = sz - 1; i > insert_pos; i--) {
    std::swap(array[i], array[i - 1]);
  }
  array[insert_pos] = value;
}


leaf_t::leaf_t(bool is_leaf_, uint32_t cap)
    : is_leaf(is_leaf_)
    , capacity(cap)
    , size(0)
    , keys(cap)
    , values(is_leaf ? cap : 0)
    /*, children(is_leaf ? 0 : cap) */{}

leaf_t ::~leaf_t() {
  keys.clear();
  values.clear();
  //children.clear();
}

bool leaf_t::insert(slice_t &k, slice_t &v) {
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

EXPORT std::optional<slice_t> leaf_t::find(const slice_t &k) const {
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
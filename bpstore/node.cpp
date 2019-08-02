#include <algorithm>
#include <bpstore/node.h>
#include <bpstore/utils/exception.h>
#include <bpstore/utils/utils.h>
#include <cstring>
#include <iterator>

using namespace bpstore;

leaf_t::leaf_t(capacity_t cap)
    : capacity(cap)
    , size(0)
    , keys(cap)
    , values(cap) {}

leaf_t ::~leaf_t() {
  keys.clear();
  values.clear();
}

bool leaf_t::insert(slice_t &k, slice_t &v) {
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

node_t::node_t(uint32_t cap)
    : capacity(cap)
    , size(0)
    , keys(cap)
    , children(cap) {}

node_t::~node_t() {
  size = 0;
  keys.clear();
  children.clear();
}

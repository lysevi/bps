#include <algorithm>
#include <bpstore/node.h>
#include <bpstore/utils/utils.h>
#include <cstring>

using namespace bpstore;

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

node_t ::~node_t() {}

bool node_t::insert(slice_t &k, slice_t &v) {
  ENSURE(is_leaf);
  ENSURE(keys.size() == capacity);
  ENSURE(values.size() == capacity);
  UNUSED(k);
  UNUSED(v);
  return false;
}
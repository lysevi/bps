#include <bpstore/node.h>
#include <bpstore/utils/utils.h>
#include <algorithm>
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
    , keys(nullptr)
    , values(nullptr)
    , children(nullptr) {

  keys = new slice_t[cap];
  if (is_leaf) {
    values = new slice_t[cap];
  } else {
    children = new storage_ptr_t[cap];
  }
}

node_t ::~node_t() {
  if (keys != nullptr) {
    delete[] keys;
  }
  if (values != nullptr) {
    delete[] values;
  }
  if (children != nullptr) {
    delete[] children;
  }
}

bool node_t::insert(slice_t &k, slice_t &v) {
  ENSURE(keys != nullptr);
  ENSURE(values != nullptr);
  UNUSED(k);
  UNUSED(v);
  return false;
}
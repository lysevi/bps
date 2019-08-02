#include <bpstore/tree.h>
#include <bpstore/utils/exception.h>

using namespace bpstore;

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

tree_t::tree_t(iblock_storage *storage)
    : _storage(storage) {
  ENSURE(storage != nullptr);
}

tree_t::~tree_t() {}
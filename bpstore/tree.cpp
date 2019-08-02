#include <bpstore/tree.h>

using namespace bpstore;

tree_t::tree_t(iblock_storage *storage)
    : _storage(storage) {}

tree_t::~tree_t() {}
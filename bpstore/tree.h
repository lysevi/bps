#pragma once

#include <bpstore/exports.h>
#include <bpstore/node.h>

namespace bpstore {

struct iblock_storage {};

struct tree_params_t {
  capacity_t node_capacity;
};

class tree_t {
public:
  EXPORT tree_t(iblock_storage *storage);
  EXPORT ~tree_t();

private:
  iblock_storage *const _storage;
};

} // namespace bpstore
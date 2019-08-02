#include <bpstore/tree.h>
#include <bpstore/utils/exception.h>
#include <bpstore/utils/utils.h>

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

tree_t::tree_t(const bpstore::utils::logging::abstract_logger_ptr &logger,
               iblock_storage *storage)
    : _storage(storage)
    , _logger(logger) {
  _logger->info("Tree start.");
  ENSURE(storage != nullptr);
}

tree_t::~tree_t() {
  _logger->info("Tree stop.");
}

bool tree_t::insert(slice_t &k, slice_t &v) {
  UNUSED(k);
  UNUSED(v);
  return true;
}

std::optional<slice_t> tree_t::find(const slice_t &k) const {
  UNUSED(k);
  return {};
}
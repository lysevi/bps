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

tree_t::tree_t(const tree_params_t &params,
               const bpstore::utils::logging::abstract_logger_ptr &logger,
               iblock_storage *storage)
    : _storage(storage)
    , _logger(logger)
    , _params(params) {
  _logger->info("start.");
  ENSURE(storage != nullptr);

  _last_leaf = _storage->load_max_leaf(_params);
  if (_last_leaf == nullptr) {
    _logger->info("create first leaf.");
    _last_leaf = _storage->create_leaf(_params);
    ENSURE(_last_leaf->address != k_storage_ptr_null);
    ENSURE(_last_leaf->capacity == _params.node_capacity);
  }
}

tree_t::~tree_t() {
  _logger->info("stop.");
}

bool tree_t::insert(const slice_t &k, const slice_t &v) {
  auto bucket = target(k);
  auto insertion_result = bucket->insert(k, v);

  if (!insertion_result) {
    THROW_EXCEPTION("tree_t::insert: ");
  }
  return insertion_result;
}

std::optional<slice_t> tree_t::find(const slice_t &k) const {
  if (_last_leaf->first_key()->compare(k) <= 0) {
    return _last_leaf->find(k);
  }
  return {};
}

leaf_ptr_t tree_t::target(const slice_t &k) const {
  if (_last_leaf->empty()
      || _last_leaf->first_key()->compare(k) <= 0) { // key >= last_leaf[0]
    return _last_leaf;
  }
  return nullptr;
}
#include <rstore/tree.h>
#include <rstore/utils/exception.h>
#include <rstore/utils/utils.h>

#include <cstring>

using namespace rstore;

namespace rstore::inner {
MemLevel::MemLevel(size_t B)
    : _keys(B)
    , _vals(B)
    , _cap(B)
    , _size(0) {
}

bool MemLevel::insert(Slice &&k, Slice &&v) {
  if (_cap == _size) {
    return false;
  }
  _keys[_size] = std::move(k);
  _vals[_size] = std::move(v);

  ++_size;
  return true;
}

std::optional<Slice> MemLevel::find(const Slice &k) const {
  for (size_t i = 0; i < _size; ++i) {
    if (k.compare(_keys[i]) == 0) {
      return _vals[i];
    }
  }
  return {};
}
} // namespace rstore::inner

Tree::Tree(const Params &p)
    : _params(p) {}

void Tree::init() {
  _memory_level = std::make_shared<inner::MemLevel>(_params.B);
}

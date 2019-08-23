#include <rstore/levels.h>

#include <rstore/bloom.h>
#include <rstore/utils/exception.h>
#include <rstore/utils/utils.h>

#include <algorithm>
#include <cstring>
#include <list>
#include <numeric>

namespace rstore::inner {
MemLevel::MemLevel(size_t B)
    : _keys(B)
    , _vals(B)
    , _cap(B)
    , _size(0) {}

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

void MemLevel::sort() {
  std::vector<size_t> indexes(_keys.size());
  std::iota(indexes.begin(), indexes.end(), 0);

  std::sort(indexes.begin(), indexes.end(), [this](const auto &kv1, const auto &kv2) {
    auto k1 = &this->_keys[kv1];
    auto k2 = &this->_keys[kv2];
    return k1->compare(*k2) < 0;
  });
  // TODO zero allocations
  std::vector<Slice> keys(_keys.size());
  std::vector<Slice> vals(_vals.size());
  size_t pos = 0;
  for (auto i : indexes) {
    keys[pos] = std::move(_keys[i]);
    vals[pos] = std::move(_vals[i]);
    ++pos;
  }
  _keys = std::move(keys);
  _vals = std::move(vals);
}

LowLevel::LowLevel(size_t B, size_t bloom_size)
    : _keys(B)
    , _vals(B)
    , _bloom_fltr(bloom_size)
    , _cap(B)
    , _size(0) {}

void LowLevel::update_header() {
  for (const auto &k : _keys) {
    rstore::inner::Bloom::add(_bloom_fltr, k.data, k.size);
  }
}

bool LowLevel::insert(Slice &&k, Slice &&v) {
  ENSURE(_cap > _size);
  _keys[_size] = std::move(k);
  _vals[_size] = std::move(v);
  rstore::inner::Bloom::add(_bloom_fltr, k.data, k.size);
  ++_size;
  return true;
}

std::optional<Slice> LowLevel::find(const Slice &k) const {
  if (!rstore::inner::Bloom::find(_bloom_fltr, k.data, k.size)) {
    return {};
  }
  auto low = std::lower_bound(
      _keys.begin(), _keys.end(), k, [](const auto &k1, const auto &k2) {
        return k1.compare(k2) < 0;
      });

  if (low == _keys.end()) {
    return {};
  }

  auto upper = std::upper_bound(
      _keys.begin(), _keys.end(), k, [](const auto &k1, const auto &k2) {
        return k1.compare(k2) < 0;
      });

  for (auto it = low; it != upper; ++it) {
    if (k.compare(*it) == 0) {
      return _vals[std::distance(_keys.begin(), it)];
    }
  }
  return {};
}

void kmerge(LowLevel *dest, std::vector<INode *> src) {
  auto vals_size = src.size();
  std::list<size_t> poses;
  for (size_t i = 0; i < vals_size; ++i) {
    poses.push_back(0);
  }
  while (!src.empty()) {
    vals_size = src.size();
    // get cur max;
    auto with_max_index = poses.begin();
    auto max_val = src.front()->at(*with_max_index);
    auto it = src.begin();
    auto with_max_index_it = it;
    for (auto pos_it = poses.begin(); pos_it != poses.end(); ++pos_it) {
      if (max_val.first->compare(*(*it)->at(*pos_it).first) >= 0) {
        with_max_index = pos_it;
        max_val = (*it)->at(*pos_it);
        with_max_index_it = it;
      }
      ++it;
    }

    auto val = (*with_max_index_it)->at(*with_max_index);
    if (dest->size() == 0 || dest->back() != val) {
      dest->push_back(val);
    }
    // remove ended in-list
    (*with_max_index)++;
    if ((*with_max_index) >= (*with_max_index_it)->size()) {
      poses.erase(with_max_index);
      src.erase(with_max_index_it);
    }
  }
  dest->update_header();
}

} // namespace rstore::inner

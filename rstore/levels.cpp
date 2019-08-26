#include <rstore/levels.h>

#include <rstore/bloom.h>
#include <rstore/utils/exception.h>
#include <rstore/utils/utils.h>

#include <algorithm>
#include <cstring>
#include <iterator>
#include <list>
#include <numeric>

namespace rstore::inner {
namespace {
template <class It, class T, class BinaryPred>
std::pair<It, It> bin_search(It &&begin, It &&end, T &&k, BinaryPred &&pred) {
  auto low = std::lower_bound(begin, end, k, pred);
  if (low != end) {
    auto upper = std::upper_bound(begin, end, k, pred);
    return std::pair{low, upper};
  }
  return std::pair(end, end);
}
} // namespace

std::optional<Link> CascadeIndex::find(const Slice &k) const {
  if (_links_pos != 0) {
    if (Bloom::find(_links_bloom_fltr, k.data, k.size)) {
      // TODO zero allocation
      Link tmp_link;
      tmp_link.key = k;
      auto [low, upper] = bin_search(
          _links.begin(), _links.end(), tmp_link, [](const auto &k1, const auto &k2) {
            return k1.key < k2.key;
          });

      for (auto it = low; it != upper; ++it) {
        if (!it->empty() && k == it->key) {
          return *it;
        }
      }
    }
  }
  return {};
}

void CascadeIndex::add_link(const Slice &k, const size_t pos, const size_t lvl) {
  _links[_links_pos++] = Link{k, pos, lvl};
  Bloom::add(_links_bloom_fltr, k.data, k.size);
}

MemLevel::MemLevel(size_t B)
    : _keys(B)
    , _vals(B)
    , _index(B)
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

std::variant<Slice, Link, bool> MemLevel::find(const Slice &k) const {
  for (size_t i = 0; i < _size; ++i) {
    if (k.compare(_keys[i]) == 0) {
      return _vals[i];
    }
  }
  auto l = _index.find(k);
  if (l.has_value()) {
    return l.value();
  } else {
    return false;
  }
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

void MemLevel::add_link(const Slice &k, const size_t pos, const size_t lvl) {
  _index.add_link(k, pos, lvl);
}

LowLevel::LowLevel(size_t num, size_t B, size_t bloom_size)
    : _num(num)
    , _keys(B)
    , _vals(B)
    , _bloom_fltr(bloom_size)
    , _index(B)
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

void LowLevel::add_link(const Slice &k, const size_t pos, const size_t lvl) {
  _index.add_link(k, pos, lvl);
}

Slice LowLevel::find(const Link &l) const {
  return _vals.at(l.pos);
}

std::variant<Slice, Link, bool> LowLevel::find(const Slice &k) const {
  if (rstore::inner::Bloom::find(_bloom_fltr, k.data, k.size)) {
    auto [low, upper]
        = bin_search(_keys.begin(), _keys.end(), k, [](const auto &k1, const auto &k2) {
            return k1 < k2;
          });

    if (low != _keys.end()) {
      for (auto it = low; it != upper; ++it) {
        if (k == *it) {
          return _vals[std::distance(_keys.begin(), it)];
        }
      }
    }
  }
  auto l = _index.find(k);
  if (l.has_value()) {
    return l.value();
  } else {
    return false;
  }
}

void kmerge(IOutLevel *dest, std::vector<ILevel *> src) {
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
      auto pos = dest->push_back(val);
      (*with_max_index_it)->add_link(*val.first, pos, dest->num());
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

#include <rstore/tree.h>
#include <rstore/utils/cz.h>
#include <rstore/utils/exception.h>
#include <rstore/utils/utils.h>

#include <algorithm>
#include <cstring>
#include <list>
#include <numeric>
using namespace rstore;

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

LowLevel::LowLevel(size_t B)
    : _keys(B)
    , _vals(B)
    , _cap(B)
    , _size(0) {}

bool LowLevel::insert(Slice &&k, Slice &&v) {
  ENSURE(_cap > _size);
  _keys[_size] = std::move(k);
  _vals[_size] = std::move(v);
  ++_size;
  return true;
}

std::optional<Slice> LowLevel::find(const Slice &k) const {
  auto low = std::lower_bound(
      _keys.begin(), _keys.end(), k, [this](const auto &k1, const auto &k2) {
        return k1.compare(k2) < 0;
      });

  auto upper = std::upper_bound(
      _keys.begin(), _keys.end(), k, [this](const auto &k1, const auto &k2) {
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
}
} // namespace rstore::inner

Tree::Tree(const Params &p)
    : _params(p) {
  _merge_iteration = 0;
}

void Tree::init() {
  _memory_level = std::make_shared<inner::MemLevel>(_params.B);
}

size_t calc_outlevel_num(size_t size_b) {
  size_t new_items_count = size_b + 1;
  return utils::ctz(~0 & new_items_count);
}

size_t block_in_level(size_t lev_num) {
  return (size_t(1) << lev_num);
}

void Tree::insert(Slice &&k, Slice &&v) {
  if (_memory_level->insert(std::move(k), std::move(v))) {
    return;
  } else {
    _memory_level->sort();
    auto out_lvl = calc_outlevel_num(_merge_iteration);
    if (_levels.size() <= out_lvl) {
      _levels.push_back(
          std::make_shared<inner::LowLevel>(block_in_level(out_lvl) * _params.B));
    }

    auto merge_target = _levels[out_lvl];
    std::vector<inner::INode *> to_merge(out_lvl + 1);
    to_merge[0] = _memory_level.get();
    for (size_t i = 0; i < out_lvl; ++i) {
      to_merge[i + 1] = _levels[i].get();
    }

    inner::kmerge(merge_target.get(), to_merge);

    this->_memory_level->clear();
    for (size_t i = 0; i < out_lvl; ++i) {
      _levels[i]->clear();
    }

    _memory_level->insert(std::move(k), std::move(v));
    _merge_iteration++;
    return;
  }
}

std::optional<Slice> Tree::find(const Slice &k) const {
  if (!_memory_level->empty()) {
    auto answer = _memory_level->find(k);
    if (answer.has_value()) {
      return answer;
    }
  }

  for (const auto &l : _levels) {
    if (!l->empty()) {
      auto answer = l->find(k);
      if (answer.has_value()) {
        return answer;
      }
    }
  }
  return {};
}
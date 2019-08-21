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
  ENSURE(_cap < _size);
  _keys[_size] = std::move(k);
  _vals[_size] = std::move(v);
  ++_size;
  return true;
}

std::optional<Slice> LowLevel::find(const Slice &k) const {
  // auto low = std::lower_bound(
  //    _keys.begin(), _keys.end(), [this](const auto &k1, const auto &k2) {
  //      return k1->compare(*k2) < 0;
  //    });

  // auto upper = std::lower_bound(
  //    _keys.begin(), _keys.end(), [this](const auto &k1, const auto &k2) {
  //      return k1->compare(*k2) < 0;
  //    });

  for (size_t i = 0; i < _size; ++i) {
    if (k.compare(_keys[i]) == 0) {
      return _vals[i];
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
      if (max_val.first->compare(*(*it)->at(*pos_it).first) <= 0) {
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
    : _params(p) {}

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

//
// template <class T, class Out, class comparer_t>
// void k_merge(std::list<T> new_values, Out &out, comparer_t comparer) {
//  auto vals_size = new_values.size();
//  std::list<size_t> poses;
//  for (size_t i = 0; i < vals_size; ++i) {
//    poses.push_back(0);
//  }
//  while (!new_values.empty()) {
//    // get cur max;
//    auto with_max_index = poses.begin();
//    auto max_val = new_values.front()->at(*with_max_index);
//    auto it = new_values.begin();
//    auto with_max_index_it = it;
//    for (auto pos_it = poses.begin(); pos_it != poses.end(); ++pos_it) {
//      if (!comparer(max_val, (*it)->at(*pos_it))) {
//        with_max_index = pos_it;
//        max_val = (*it)->at(*pos_it);
//        with_max_index_it = it;
//      }
//      ++it;
//    }
//
//    auto val = (*with_max_index_it)->at(*with_max_index);
//    if (out->size() == 0 || out->back() != val) {
//      out.push_back(val);
//    }
//    // remove ended in-list
//    (*with_max_index)++;
//    if ((*with_max_index) >= (*with_max_index_it)->size()) {
//      poses.erase(with_max_index);
//      new_values.erase(with_max_index_it);
//    }
//  }
//}

void Tree::insert(Slice &&k, Slice &&v) {
  if (_memory_level->insert(std::move(k), std::move(v))) {
    return;
  } else {
    _memory_level->sort();
    auto out_lvl = calc_outlevel_num(_levels.size());
    if (_levels.size() < out_lvl || _levels.empty()) {
      _levels.push_back(
          std::make_shared<inner::LowLevel>(block_in_level(out_lvl) * _params.B));
    }

    /*auto merge_target = _levels[out_lvl];
    std::list<inner::MemLevelPtr> to_merge;
    to_merge.push_back(_memory_level);
    for (size_t i = 0; i < out_lvl; ++i) {
      to_merge.push_back(_levels[i]);
    }*/

    /*k_merge(to_merge, merge_target, [](const Slice &s1, const Slice &s2) {
      return s1.compare(s2) > 0;
    });*/
    return;
  }
}

std::optional<Slice> Tree::find(const Slice &k) const {
  return _memory_level->find(k);
}
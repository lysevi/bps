#include <rstore/tree.h>
#include <rstore/levels.h>
#include <rstore/utils/cz.h>
#include <rstore/utils/exception.h>
#include <rstore/utils/utils.h>

#include <algorithm>
#include <cstring>
#include <list>
#include <numeric>
using namespace rstore;

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
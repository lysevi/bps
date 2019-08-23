#include <rstore/levels.h>
#include <rstore/tree.h>
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
      _levels.push_back(std::make_shared<inner::LowLevel>(
          out_lvl, block_in_level(out_lvl) * _params.B, _params.BloomSize));
    }

    auto merge_target = _levels[out_lvl];
    _memory_level->clear_link();
    std::vector<inner::INode *> to_merge(out_lvl + 1);
    to_merge[0] = _memory_level.get();
    for (size_t i = 0; i < out_lvl; ++i) {
      auto ptr = _levels[i].get();
      ptr->clear_link();
      to_merge[i + 1] = ptr;
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
  std::optional<Slice> result;
  std::optional<inner::Link> target_lvl;
  auto answer_clbk = [&](auto &&arg) {
    using T = std::decay_t<decltype(arg)>;
    if constexpr (std::is_same_v<T, Slice>) {
      result = arg;
    }
    if constexpr (std::is_same_v<T, inner::Link>) {
      target_lvl = arg;
    }
    if constexpr (std::is_same_v<T, bool>) {
    }
  };

  if (!_memory_level->empty()) {
    auto answer = _memory_level->find(k);

    std::visit(answer_clbk, answer);
  }

  if (result.has_value()) {
    return result;
  }

  if (target_lvl.has_value()) {
    auto link = target_lvl.value();
    auto l = _levels[link.lvl];
    return l->find(k, link.pos);
  }

  for (const auto &l : _levels) {
    if (!l->empty()) {
      auto answer = l->find(k);
      std::visit(answer_clbk, answer);

      if (result.has_value()) {
        return result;
      }

      if (target_lvl.has_value()) {
        auto link = target_lvl.value();
        auto lvl = _levels[link.lvl];
        return lvl->find(k, link.pos);
      }
    }
  }
  return {};
}
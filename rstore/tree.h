#pragma once

#include <rstore/exports.h>
#include <rstore/slice.h>
#include <rstore/utils/exception.h>

#include <cstdint>
#include <optional>
#include <utility>
#include <vector>

namespace rstore {

namespace inner {
struct MemLevel;
struct LowLevel;

using MemLevelPtr = std::shared_ptr<MemLevel>;
using LowLevelPtr = std::shared_ptr<LowLevel>;
}; // namespace inner

class Tree {
public:
  struct Params {
    size_t B = 10;
    size_t BloomSize = 10;
  };
  EXPORT Tree(const Params &p);
  EXPORT void init();
  EXPORT void insert(Slice &&k, Slice &&v);
  EXPORT std::optional<Slice> find(const Slice &k) const;
  size_t deep() const { return _levels.size(); }

protected:
  Params _params;
  inner::MemLevelPtr _memory_level;
  std::vector<inner::LowLevelPtr> _levels;
  size_t _merge_iteration;
};
}; // namespace rstore
#pragma once

#include <rstore/exports.h>
#include <rstore/slice.h>
#include <rstore/utils/exception.h>

#include <cstdint>
#include <optional>
#include <vector>

namespace rstore {

namespace inner {
struct MemLevel {
  EXPORT MemLevel(size_t B);
  EXPORT bool insert(Slice &&k, Slice &&v);
  EXPORT std::optional<Slice> find(const Slice &k) const;
  EXPORT void sort();
  std::vector<std::pair<Slice, Slice>> _kv;
  const size_t _cap;
  size_t _size;
};

using MemLevelPtr = std::shared_ptr<MemLevel>;
}; // namespace inner

class Tree {
public:
  struct Params {
    size_t B;
  };
  EXPORT Tree(const Params &p);
  EXPORT void init();
  EXPORT void insert(Slice &&k, Slice &&v);
  EXPORT std::optional<Slice> find(const Slice &k) const;

protected:
  Params _params;
  inner::MemLevelPtr _memory_level;
  std::vector<inner::MemLevelPtr> _levels;
};
}; // namespace rstore
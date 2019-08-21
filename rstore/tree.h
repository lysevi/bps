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
struct INode {
  virtual ~INode() {}
  virtual bool insert(Slice &&k, Slice &&v) = 0;
  virtual size_t size() const = 0;
  virtual std::pair<Slice *, Slice *> at(size_t s) = 0;
  virtual bool empty() const = 0;
  virtual void clear() = 0;
};

struct MemLevel : public INode {
  EXPORT MemLevel(size_t B);
  EXPORT bool insert(Slice &&k, Slice &&v) override;
  EXPORT std::optional<Slice> find(const Slice &k) const;
  EXPORT void sort();

  size_t size() const override { return _size; }
  std::pair<Slice *, Slice *> at(size_t s) override {
    return std::pair(&_keys.at(s), &_vals.at(s));
  }
  bool empty() const override { return _size == 0; }

  void clear() override {
    for (size_t i = 0; i < _size; ++i) {
      _keys[i] = Slice();
      _vals[i] = Slice();
    }
    _size = 0;
  }
  std::vector<Slice> _keys;
  std::vector<Slice> _vals;
  const size_t _cap;
  size_t _size;
};

struct LowLevel : public INode {
  EXPORT LowLevel(size_t B);
  EXPORT bool insert(Slice &&k, Slice &&v) override;
  EXPORT std::optional<Slice> find(const Slice &k) const;

  size_t size() const override { return _size; }
  std::pair<Slice *, Slice *> at(size_t s) override {
    return std::pair(&_keys.at(s), &_vals.at(s));
  }

  void clear() override {
    for (size_t i = 0; i < _size; ++i) {
      _keys[i] = Slice();
      _vals[i] = Slice();
    }
    _size = 0;
  }

  std::pair<Slice *, Slice *> back() {
    if (_size == 0) {
      return std::pair<Slice *, Slice *>(nullptr, nullptr);
    } else {
      return at(_size - 1);
    }
  }
  void push_back(std::pair<Slice *, Slice *> vals) {
    _keys[_size] = *vals.first;
    _vals[_size] = *vals.second;
    _size++;
  }
  bool empty() const override { return _size == 0; }
  std::vector<Slice> _keys;
  std::vector<Slice> _vals;
  const size_t _cap;
  size_t _size;
};

using MemLevelPtr = std::shared_ptr<MemLevel>;
using LowLevelPtr = std::shared_ptr<LowLevel>;

EXPORT void kmerge(LowLevel *dest, std::vector<INode *> src);
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
  size_t deep() const { return _levels.size(); }

protected:
  Params _params;
  inner::MemLevelPtr _memory_level;
  std::vector<inner::LowLevelPtr> _levels;
  size_t _merge_iteration;
};
}; // namespace rstore
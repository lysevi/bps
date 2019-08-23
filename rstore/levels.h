#pragma once

#pragma once

#include <rstore/exports.h>
#include <rstore/slice.h>
#include <rstore/utils/exception.h>

#include <cstdint>
#include <utility>
#include <variant>
#include <vector>

namespace rstore::inner {
const size_t UNKNOW_POS = std::numeric_limits<size_t>::max();
const size_t UNKNOW_LVL = std::numeric_limits<size_t>::max();

struct Link {
  Slice key;
  size_t pos = UNKNOW_POS;
  size_t lvl = UNKNOW_LVL; // TODO move to level header.

  bool empty() const { return key.empty() || pos == UNKNOW_POS || lvl == UNKNOW_LVL; }
};

struct INode {
  virtual ~INode() {}
  virtual bool insert(Slice &&k, Slice &&v) = 0;
  virtual std::variant<Slice, Link, bool> find(const Slice &k) const = 0;
  virtual size_t size() const = 0;
  virtual std::pair<Slice *, Slice *> at(size_t s) = 0;
  virtual bool empty() const = 0;
  virtual void clear() = 0;
  virtual void clear_link() = 0;
  virtual void add_link(const Slice &k, const size_t pos, const size_t lvl) = 0;
};

struct MemLevel : public INode {
  EXPORT MemLevel(size_t B);
  EXPORT bool insert(Slice &&k, Slice &&v) override;
  EXPORT std::variant<Slice, Link, bool> find(const Slice &k) const override;
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
  void clear_link() override {
    for (auto &l : _links) {
      l = Link();
    }
  }
  void add_link(const Slice &k, const size_t pos, const size_t lvl) override {
    for (size_t i = 0; i < _links.size(); ++i) {
      if (_links[i].pos == UNKNOW_POS) {
        _links[i] = Link{k, pos, lvl};
        break;
      }
    }
  }
  std::vector<Slice> _keys;
  std::vector<Slice> _vals;
  std::vector<Link> _links;
  const size_t _cap;
  size_t _size;
};

struct LowLevel : public INode {

  EXPORT LowLevel(size_t num, size_t B, size_t bloom_size);
  EXPORT bool insert(Slice &&k, Slice &&v) override;
  EXPORT Slice find(const Slice &k, size_t pos) const;
  EXPORT std::variant<Slice, Link, bool> find(const Slice &k) const override;

  size_t size() const override { return _size; }
  std::pair<Slice *, Slice *> at(size_t s) override {
    return std::pair(&_keys.at(s), &_vals.at(s));
  }

  void add_link(const Slice &k, const size_t pos, const size_t lvl) override {
    for (size_t i = 0; i < _links.size(); ++i) {
      if (_links[i].pos == UNKNOW_POS) {
        _links[i] = Link{k, pos, lvl};
        break;
      }
    }
  }

  void clear() override {
    for (size_t i = 0; i < _size; ++i) {
      _keys[i] = Slice();
      _vals[i] = Slice();
    }
    std::fill(_bloom_fltr.begin(), _bloom_fltr.end(), false);
    _size = 0;
  }

  void clear_link() override {
    for (auto &l : _links) {
      l = Link();
    }
  }

  std::pair<Slice *, Slice *> back() {
    if (_size == 0) {
      return std::pair<Slice *, Slice *>(nullptr, nullptr);
    } else {
      return at(_size - 1);
    }
  }

  size_t push_back(std::pair<Slice *, Slice *> vals) {
    auto pos = _size;
    _keys[_size] = *vals.first;
    _vals[_size] = *vals.second;
    _size++;
    return pos;
  }

  bool empty() const override { return _size == 0; }

  void update_header();
  size_t _num = 0;
  std::vector<Slice> _keys;
  std::vector<Slice> _vals;
  std::vector<bool> _bloom_fltr;
  std::vector<Link> _links;
  const size_t _cap;
  size_t _size;
};

EXPORT void kmerge(LowLevel *dest, std::vector<INode *> src);
}; // namespace rstore::inner

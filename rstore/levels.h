#pragma once

#pragma once

#include <rstore/exports.h>
#include <rstore/slice.h>
#include <rstore/utils/exception.h>

#include <cstdint>
#include <optional>
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

struct CascadeIndex {
  CascadeIndex(size_t B)
      : _links(B)
      , _links_bloom_fltr(B) {}

  void clear_link() {
    for (auto &l : _links) {
      l = Link();
    }
    std::fill(_links_bloom_fltr.begin(), _links_bloom_fltr.end(), false);
    _links_pos = 0;
  }

  void add_link(const Slice &k, const size_t pos, const size_t lvl);
  std::optional<Link> find(const Slice &k) const;

  bool empty() const { return _links_pos == 0; }
  std::vector<Link> _links;
  std::vector<bool> _links_bloom_fltr;
  size_t _links_pos = 0;
};

struct IKvStore {
  virtual std::variant<Slice, Link, bool> find(const Slice &k) const = 0;
  virtual bool insert(Slice &&k, Slice &&v) = 0;
};

struct ILevel {
  virtual ~ILevel() {}
  virtual size_t size() const = 0;
  virtual std::pair<Slice *, Slice *> at(size_t s) = 0;
  virtual bool empty() const = 0;
  virtual void clear() = 0;
  virtual void clear_link() = 0;
  virtual void add_link(const Slice &k, const size_t pos, const size_t lvl) = 0;
};

struct IOutLevel : public ILevel {
  virtual size_t push_back(std::pair<Slice *, Slice *> vals) = 0;
  virtual std::pair<Slice *, Slice *> back() = 0;
  virtual size_t num() const = 0;
  virtual void update_header() = 0;
};

struct MemLevel final : public ILevel, public IKvStore {
  EXPORT MemLevel(size_t B);
  EXPORT bool insert(Slice &&k, Slice &&v) override;
  EXPORT std::variant<Slice, Link, bool> find(const Slice &k) const override;
  EXPORT void sort();

  size_t size() const override { 
	  return _size; 
  }
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
  void clear_link() override { _index.clear_link(); }
  EXPORT void add_link(const Slice &k, const size_t pos, const size_t lvl) override;
  std::vector<Slice> _keys;
  std::vector<Slice> _vals;
  CascadeIndex _index;
  const size_t _cap;
  size_t _size;
};

struct LowLevel final : public IOutLevel, public IKvStore {

  EXPORT LowLevel(size_t num, size_t B, size_t bloom_size);
  EXPORT bool insert(Slice &&k, Slice &&v) override;
  EXPORT std::variant<Slice, Link, bool> find(const Slice &k) const override;
  EXPORT Slice find(const Link &l) const;

  size_t size() const override { return _size; }
  std::pair<Slice *, Slice *> at(size_t s) override {
    return std::pair(&_keys.at(s), &_vals.at(s));
  }

  EXPORT void add_link(const Slice &k, const size_t pos, const size_t lvl) override;

  void clear() override {
    for (size_t i = 0; i < _size; ++i) {
      _keys[i] = Slice();
      _vals[i] = Slice();
    }
    std::fill(_bloom_fltr.begin(), _bloom_fltr.end(), false);
    _size = 0;
  }

  void clear_link() override { _index.clear_link(); }

  std::pair<Slice *, Slice *> back() override {
    if (_size == 0) {
      return std::pair<Slice *, Slice *>(nullptr, nullptr);
    } else {
      return at(_size - 1);
    }
  }

  size_t push_back(std::pair<Slice *, Slice *> vals) override {
    auto pos = _size;
    _keys[_size] = *vals.first;
    _vals[_size] = *vals.second;
    _size++;
    return pos;
  }

  bool empty() const override { return _size == 0 && _index.empty(); }
  size_t num() const override { return _num; }
  void update_header() override;

  size_t _num = 0;
  std::vector<Slice> _keys;
  std::vector<Slice> _vals;
  std::vector<bool> _bloom_fltr;
  CascadeIndex _index;
  const size_t _cap;
  size_t _size;
};

EXPORT void kmerge(IOutLevel *dest, std::vector<ILevel *> src);
}; // namespace rstore::inner

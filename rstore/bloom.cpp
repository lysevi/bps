#include <rstore/bloom.h>

namespace rstore::inner {
namespace {
uint64_t jenkins_hash(uint8_t *key, size_t len) {
  uint64_t hash, i;
  for (hash = i = 0; i < len; ++i) {
    hash += key[i];
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }
  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);
  return hash;
}
uint64_t std_hasher(uint8_t *data, size_t data_size) {
  std::hash<uint8_t> hasher;
  uint64_t result = 0;
  for (size_t i = 0; i < data_size; ++i) {
    result = (result << 1) ^ hasher(data[i]);
  }
  return result;
}
} // namespace

void Bloom::add(std::vector<bool> &fltr, uint8_t *data, size_t data_size) {
  {
    auto h = jenkins_hash(data, data_size);
    fltr[h % fltr.size()] = true;
  }
  {
    auto h = std_hasher(data, data_size);
    fltr[h % fltr.size()] = true;
  }
}

bool Bloom::find(const std::vector<bool> &fltr, uint8_t *data, size_t data_size) {
  {
    auto h = jenkins_hash(data, data_size);
    if (fltr[h % fltr.size()] == false) {
      return false;
    }
  }
  {
    auto h = std_hasher(data, data_size);
    if (fltr[h % fltr.size()] == false) {
      return false;
    }
  }
  return true;
}

} // namespace rstore::inner
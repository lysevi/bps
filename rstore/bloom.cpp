#include <rstore/bloom.h>

namespace rstore {

uint64_t jenkins_one_at_a_time_hash(uint8_t *key, size_t len) {
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

Bloom::Bloom(bool *fltr_, size_t size_)
    : fltr(fltr_)
    , s(size_) {
  functions = {[](uint8_t *data, size_t data_size) {
                 return jenkins_one_at_a_time_hash(data, data_size);
               },
               [](uint8_t *data, size_t data_size) {
                 std::hash<uint8_t> hasher;
                 uint64_t result = 0;
                 for (size_t i = 0; i < data_size; ++i) {
                   result = (result << 1) ^ hasher(data[i]);
                 }
                 return result;
               }};
}

void Bloom::add(uint8_t *data, size_t data_size) {
  for (const auto &f : functions) {
    auto h = f(data, data_size);
    fltr[h % s] = true;
  }
}

bool Bloom::find(uint8_t *data, size_t data_size) const {
  for (auto &f : functions) {
    auto h = f(data, data_size);
    if (fltr[h % s] == false) {
      return false;
    }
  }
  return true;
}

} // namespace rstore
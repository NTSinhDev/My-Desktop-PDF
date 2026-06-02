#include <mdpdf/infrastructure/cache/cache_key.hpp>

namespace mdpdf::infra {

bool CacheKey::operator==(const CacheKey& other) const {
  return document == other.document && page == other.page &&
         scale_milli == other.scale_milli && dpr_milli == other.dpr_milli;
}

std::size_t CacheKeyHash::operator()(const CacheKey& key) const noexcept {
  std::size_t h = std::hash<core::DocumentHandle>{}(key.document);
  h ^= std::hash<int>{}(key.page) << 1;
  h ^= std::hash<int>{}(key.scale_milli) << 2;
  h ^= std::hash<int>{}(key.dpr_milli) << 3;
  return h;
}

}  // namespace mdpdf::infra

#pragma once

#include <mdpdf/core/domain/document_models.hpp>
#include <mdpdf/infrastructure/cache/cache_key.hpp>

#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>

namespace mdpdf::infra {

class LruPageCache {
public:
  explicit LruPageCache(std::size_t max_entries = 48,
                        std::size_t max_bytes = 256 * 1024 * 1024);

  std::optional<core::PageBitmap> get(const CacheKey& key);
  void put(const CacheKey& key, core::PageBitmap bitmap);
  void clear();
  void invalidate_document(core::DocumentHandle doc);

  std::size_t entry_count() const;
  std::size_t byte_count() const;

private:
  void evict_if_needed();

  mutable std::mutex mutex_;
  std::size_t max_entries_;
  std::size_t max_bytes_;
  std::size_t current_bytes_ = 0;
  std::list<CacheKey> lru_order_;
  std::unordered_map<CacheKey, std::pair<core::PageBitmap, std::list<CacheKey>::iterator>,
                     CacheKeyHash>
      entries_;
};

}  // namespace mdpdf::infra

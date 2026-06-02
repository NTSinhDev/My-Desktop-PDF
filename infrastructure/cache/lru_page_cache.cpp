#include <mdpdf/infrastructure/cache/lru_page_cache.hpp>

namespace mdpdf::infra {

namespace {
std::size_t bitmap_bytes(const core::PageBitmap& bmp) {
  return bmp.bgra.size();
}
}  // namespace

LruPageCache::LruPageCache(std::size_t max_entries, std::size_t max_bytes)
    : max_entries_(max_entries), max_bytes_(max_bytes) {}

std::optional<core::PageBitmap> LruPageCache::get(const CacheKey& key) {
  std::scoped_lock lock(mutex_);
  const auto it = entries_.find(key);
  if (it == entries_.end()) {
    return std::nullopt;
  }
  lru_order_.erase(it->second.second);
  lru_order_.push_front(key);
  it->second.second = lru_order_.begin();
  return it->second.first;
}

void LruPageCache::put(const CacheKey& key, core::PageBitmap bitmap) {
  std::scoped_lock lock(mutex_);
  const auto existing = entries_.find(key);
  if (existing != entries_.end()) {
    current_bytes_ -= bitmap_bytes(existing->second.first);
    lru_order_.erase(existing->second.second);
    entries_.erase(existing);
  }

  lru_order_.push_front(key);
  entries_.emplace(key, std::make_pair(std::move(bitmap), lru_order_.begin()));
  current_bytes_ += bitmap_bytes(entries_.at(key).first);
  evict_if_needed();
}

void LruPageCache::clear() {
  std::scoped_lock lock(mutex_);
  entries_.clear();
  lru_order_.clear();
  current_bytes_ = 0;
}

void LruPageCache::invalidate_document(core::DocumentHandle doc) {
  std::scoped_lock lock(mutex_);
  for (auto it = entries_.begin(); it != entries_.end();) {
    if (it->first.document == doc) {
      current_bytes_ -= bitmap_bytes(it->second.first);
      lru_order_.erase(it->second.second);
      it = entries_.erase(it);
    } else {
      ++it;
    }
  }
}

std::size_t LruPageCache::entry_count() const {
  std::scoped_lock lock(mutex_);
  return entries_.size();
}

std::size_t LruPageCache::byte_count() const {
  std::scoped_lock lock(mutex_);
  return current_bytes_;
}

void LruPageCache::evict_if_needed() {
  while (!lru_order_.empty() &&
         (entries_.size() > max_entries_ || current_bytes_ > max_bytes_)) {
    const CacheKey key = lru_order_.back();
    lru_order_.pop_back();
    const auto it = entries_.find(key);
    if (it != entries_.end()) {
      current_bytes_ -= bitmap_bytes(it->second.first);
      entries_.erase(it);
    }
  }
}

}  // namespace mdpdf::infra

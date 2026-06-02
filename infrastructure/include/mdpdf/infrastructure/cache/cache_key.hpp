#pragma once

#include <mdpdf/core/domain/document_models.hpp>

#include <cstdint>
#include <functional>

namespace mdpdf::infra {

struct CacheKey {
  core::DocumentHandle document = core::kInvalidDocumentHandle;
  int page = 0;
  int scale_milli = 1000;
  int dpr_milli = 1000;

  bool operator==(const CacheKey& other) const;
};

struct CacheKeyHash {
  std::size_t operator()(const CacheKey& key) const noexcept;
};

}  // namespace mdpdf::infra

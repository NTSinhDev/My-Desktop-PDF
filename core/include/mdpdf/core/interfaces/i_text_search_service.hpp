#pragma once

#include <mdpdf/core/domain/document_models.hpp>

#include <functional>
#include <vector>

namespace mdpdf::core {

using SearchProgressCallback = std::function<void(int page, int total)>;
using SearchCompleteCallback = std::function<void(std::vector<SearchHit>)>;

class ITextSearchService {
public:
  virtual ~ITextSearchService() = default;
  virtual void search_async(SearchQuery query, SearchProgressCallback on_progress,
                            SearchCompleteCallback on_complete) = 0;
  virtual void cancel() = 0;
};

}  // namespace mdpdf::core

#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace mdpdf::core {

using DocumentHandle = std::uint64_t;
constexpr DocumentHandle kInvalidDocumentHandle = 0;

struct DocumentId {
  std::string value;
};

struct PageIndex {
  int value = 0;
};

struct PageLayout {
  double width_points = 0.0;
  double height_points = 0.0;
  int rotation = 0;
};

struct PdfDocumentInfo {
  DocumentHandle handle = kInvalidDocumentHandle;
  DocumentId id;
  std::filesystem::path path;
  int page_count = 0;
  std::string title;
  bool encrypted = false;
};

struct RenderRequest {
  DocumentHandle document = kInvalidDocumentHandle;
  PageIndex page;
  double scale = 1.0;
  double device_pixel_ratio = 1.0;
};

struct PageBitmap {
  int width = 0;
  int height = 0;
  int stride = 0;
  std::vector<std::uint8_t> bgra;
};

struct SearchQuery {
  DocumentHandle document = kInvalidDocumentHandle;
  std::string text;
  bool case_sensitive = false;
  bool whole_word = false;
};

struct Rect {
  double left = 0.0;
  double top = 0.0;
  double right = 0.0;
  double bottom = 0.0;
};

struct SearchHit {
  PageIndex page;
  int char_index = 0;
  std::vector<Rect> rects;
};

struct TextSelection {
  PageIndex page;
  int start_char = 0;
  int end_char = 0;
  std::string utf8_text;
};

struct Bookmark {
  std::int64_t id = 0;
  std::filesystem::path document_path;
  PageIndex page;
  std::string title;
  std::chrono::system_clock::time_point created_at{};
};

struct RecentEntry {
  std::filesystem::path path;
  std::chrono::system_clock::time_point last_opened_at{};
  int page_hint = 0;
};

DocumentId make_document_id(const std::filesystem::path& path);

}  // namespace mdpdf::core

#pragma once

#include <filesystem>

#include <spdlog/spdlog.h>

#define MDPDF_LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define MDPDF_LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define MDPDF_LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)

namespace mdpdf::infra {

void init_logging(const std::filesystem::path& log_file);

}  // namespace mdpdf::infra

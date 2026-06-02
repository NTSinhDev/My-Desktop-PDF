#include <mdpdf/infrastructure/logging/logger.hpp>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace mdpdf::infra {

void init_logging(const std::filesystem::path& log_file) {
  std::vector<spdlog::sink_ptr> sinks;
  sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
  if (!log_file.empty()) {
    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        log_file.string(), 1024 * 1024, 3));
  }
  auto logger = std::make_shared<spdlog::logger>("mdpdf", sinks.begin(),
                                                 sinks.end());
  logger->set_level(spdlog::level::info);
  spdlog::set_default_logger(logger);
}

}  // namespace mdpdf::infra

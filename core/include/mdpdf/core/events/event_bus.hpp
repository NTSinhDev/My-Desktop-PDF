#pragma once

#include <mdpdf/core/events/i_event_bus.hpp>

namespace mdpdf::core {

class EventBus final : public IEventBus {
protected:
  void subscribe_impl(std::type_index type,
                      std::function<void(const void*)> handler) override;
  void publish_impl(std::type_index type, const void* event) override;

private:
  std::unordered_map<std::type_index, std::vector<std::function<void(const void*)>>>
      handlers_;
};

}  // namespace mdpdf::core

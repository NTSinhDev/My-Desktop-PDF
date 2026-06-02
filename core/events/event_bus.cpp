#include <mdpdf/core/events/event_bus.hpp>

namespace mdpdf::core {

void EventBus::subscribe_impl(std::type_index type,
                              std::function<void(const void*)> handler) {
  handlers_[type].push_back(std::move(handler));
}

void EventBus::publish_impl(std::type_index type, const void* event) {
  const auto it = handlers_.find(type);
  if (it == handlers_.end()) {
    return;
  }
  for (const auto& handler : it->second) {
    handler(event);
  }
}

}  // namespace mdpdf::core

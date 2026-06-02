#pragma once

#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace mdpdf::core {

class IEventBus {
public:
  virtual ~IEventBus() = default;

  template <typename Event>
  void subscribe(std::function<void(const Event&)> handler) {
    subscribe_impl(typeid(Event), [handler](const void* e) {
      handler(*static_cast<const Event*>(e));
    });
  }

  template <typename Event>
  void publish(const Event& event) {
    publish_impl(typeid(Event), &event);
  }

protected:
  virtual void subscribe_impl(std::type_index type,
                              std::function<void(const void*)> handler) = 0;
  virtual void publish_impl(std::type_index type, const void* event) = 0;
};

}  // namespace mdpdf::core

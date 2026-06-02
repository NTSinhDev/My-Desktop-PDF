#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace mdpdf::infra {

class RenderThreadPool {
public:
  explicit RenderThreadPool(std::size_t thread_count = 3);
  ~RenderThreadPool();

  void enqueue(std::function<void()> task);

private:
  void worker_loop();

  std::vector<std::thread> workers_;
  std::queue<std::function<void()>> tasks_;
  std::mutex mutex_;
  std::condition_variable cv_;
  bool stop_ = false;
};

}  // namespace mdpdf::infra

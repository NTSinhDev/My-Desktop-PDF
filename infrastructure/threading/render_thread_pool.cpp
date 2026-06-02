#include <mdpdf/infrastructure/threading/render_thread_pool.hpp>

namespace mdpdf::infra {

RenderThreadPool::RenderThreadPool(std::size_t thread_count) {
  for (std::size_t i = 0; i < thread_count; ++i) {
    workers_.emplace_back([this] { worker_loop(); });
  }
}

RenderThreadPool::~RenderThreadPool() {
  {
    std::scoped_lock lock(mutex_);
    stop_ = true;
  }
  cv_.notify_all();
  for (auto& worker : workers_) {
    if (worker.joinable()) {
      worker.join();
    }
  }
}

void RenderThreadPool::enqueue(std::function<void()> task) {
  {
    std::scoped_lock lock(mutex_);
    tasks_.push(std::move(task));
  }
  cv_.notify_one();
}

void RenderThreadPool::worker_loop() {
  while (true) {
    std::function<void()> task;
    {
      std::unique_lock lock(mutex_);
      cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
      if (stop_ && tasks_.empty()) {
        return;
      }
      task = std::move(tasks_.front());
      tasks_.pop();
    }
    task();
  }
}

}  // namespace mdpdf::infra

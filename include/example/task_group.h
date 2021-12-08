#pragma once

#include <vector>
#include <future>


using TaskType = std::packaged_task<void()>;


namespace tg_naive {
/// The simplest implementation: just run each task in its own thread.
/// Conceptually the same as implementation of ProcessThreads{Flat,Nested}.

class TaskGroup {
public:
  template<class Func>
  void Schedule(Func&& f) {
    tasks_.emplace_back(std::move(f));
  }

  void RunScheduled();

private:
  std::vector<TaskType> tasks_;
};

} // namespace tg_naive

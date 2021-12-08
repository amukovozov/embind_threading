#include <example/task_group.h>


namespace tg_naive {

void TaskGroup::RunScheduled() {
  std::vector<std::thread> threads;
  threads.reserve(tasks_.size());
  for (auto& task : tasks_) {
    threads.emplace_back(std::move(task));
  }
  for (auto& thr : threads) {
    thr.join();
  }
  tasks_.clear();
}

} // namespace tg_naive

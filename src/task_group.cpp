#include <example/task_group.h>

#include <atomic>


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


static void MetaTask(
    std::vector<TaskType>& tasks,
    std::atomic_size_t&    task_idx) {
  while (true) {
    const size_t cur_task_idx = task_idx.fetch_add(1);
    if (cur_task_idx >= tasks.size()) {
      return;
    }
    tasks[cur_task_idx]();
  }
}


static void MetataskRun(std::vector<TaskType>& tasks) {
  constexpr size_t N_THREADS = POOL_SIZE;

  std::thread threads[N_THREADS];

  size_t n_workers = std::min(N_THREADS, tasks.size());

  std::atomic_size_t task_idx(0);

  for (size_t i = 0; i < n_workers; ++i) {
    threads[i] = std::thread(MetaTask, std::ref(tasks), std::ref(task_idx));
  }
  MetaTask(tasks, task_idx);
  for (size_t i = 0; i < n_workers; ++i) {
    threads[i].join();
  }
}


namespace tg_metatask_simple {

void TaskGroup::RunScheduled() {
  MetataskRun(tasks_);
  tasks_.clear();
}

} // namespace tg_metatask_simple

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


namespace tg_metatask_simple {
/// Runs POOL_SIZE MetaTasks in threads, then runs one syncronously.
/// Each MetaTask runs tasks in a loop (by atomic index).
/// This makes ProcessTGFlat work when N_VALUES > POOL_SIZE

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

} // namespace tg_metatask_simple


namespace tg_metatask_toplevel_only {
/// If there are no previous TaskGroups in the call stack, runs MetaTasks.
/// Otherwise, just runs the tasks sequentially.
/// This makes ProcessTGNested work, but not as fast as ProcessTGFlat
/// (since it only uses N_IMMEDIATE_THREADS + 1 MetaTasks).

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

} // namespace tg_metatask_toplevel_only

#include <example/example.h>

#include <utility>
#include <string>
#include <algorithm>
#include <thread>
#include <iostream>

#include <example/task_group.h>

constexpr uint64_t N_ARRAYS = 2;

constexpr uint64_t N_VALUES = 6;

constexpr uint64_t N_FLAT_THREADS = N_VALUES;

constexpr uint64_t N_IMMEDIATE_THREADS = 2;
constexpr uint64_t N_NESTED_THREADS = 3;

static_assert(N_IMMEDIATE_THREADS * N_NESTED_THREADS == N_VALUES,
    "check values count");
static_assert(
    N_IMMEDIATE_THREADS + N_IMMEDIATE_THREADS * N_NESTED_THREADS <= POOL_SIZE,
    "check sufficient thread pool size");


using namespace tg_metatask_toplevel_only;


static void Output(const std::string& prefix, uint64_t p_array[N_VALUES]) {
  std::cout << prefix;
  for (uint64_t i = 0; i < N_VALUES; ++i) {
    std::cout << p_array[i] << " ";
  }
  std::cout << std::endl;
}


/// spend some time, then give some resulting value
static uint64_t MockLongProcess(uint64_t n) {
  /// specific processing here is not really important
  uint64_t res = 0;
  for (uint64_t i = 0; i < n; ++i) {
    /// volatile so that the loop doesn't get optimized away
    volatile uint64_t delta = i * i;
    res += delta;
  }
  return res;
}


static void ProcessOneValueInplace(uint64_t* p_value) {
  *p_value = MockLongProcess(*p_value);
}


/// get some single value as a result for two arrays (no threading here)
static uint32_t GetFinalResult(uint64_t p_arrays[N_ARRAYS][N_VALUES]) {
  /// specific processing here is not really important
  uint64_t result = 0;
  for (uint64_t i = 0; i < N_ARRAYS; ++i) {
    uint64_t subresult = 0;
    for (uint64_t j = 0; j < N_VALUES; ++j) {
      subresult ^= p_arrays[i][j];
    }
    result += subresult;
  }
//  for (uint64_t i = 0; i < N_VALUES; ++i) {
//    uint64_t mx = std::max(p_values_1[i], p_values_0[i]);
//    uint64_t mn = std::min(p_values_1[i], p_values_0[i]);
//    result += mx - mn;
//  }
  return static_cast<uint32_t>(result);
}


/// THIS WORKS FINE
uint32_t Processor::ProcessNoThreads(uint32_t n) {
  std::cout << "START ProcessNoThreads" << std::endl;

  uint64_t arrays[N_ARRAYS][N_VALUES];

  for (uint64_t i = 0; i < N_ARRAYS; ++i) {
    for (uint64_t j = 0; j < N_VALUES; ++j) {
      arrays[i][j] = n + i + j;
    }
  }

  for (uint64_t i = 0; i < N_ARRAYS; ++i) {
    if (!i) {
      Output("Before processing:   ", arrays[i]);
    } else {
      Output("                     ", arrays[i]);
    }
  }

  for (uint64_t i = 0; i < N_ARRAYS; ++i) {
    for (uint64_t j = 0; j < N_VALUES; ++j) {
      ProcessOneValueInplace(&arrays[i][j]);
    }
    std::string prefix = "arrays[" + std::to_string(i) + "] processed: ";
    Output(prefix, arrays[i]);
  }

  std::cout << std::endl;

  return GetFinalResult(arrays);
}


/// THIS ALSO WORKS
uint32_t Processor::ProcessThreadsFlat(uint32_t n) {
  std::cout << "START ProcessThreadsFlat" << std::endl;;

  uint64_t arrays[N_ARRAYS][N_VALUES];

  for (uint64_t i = 0; i < N_ARRAYS; ++i) {
    for (uint64_t j = 0; j < N_VALUES; ++j) {
      arrays[i][j] = n + i + j;
    }
  }

  std::thread threads[N_FLAT_THREADS];

  auto process = [](uint64_t* p_value) {
    ProcessOneValueInplace(p_value);
  };

  for (uint64_t i = 0; i < N_ARRAYS; ++i) {
    if (!i) {
      Output("Before processing:   ", arrays[i]);
    } else {
      Output("                     ", arrays[i]);
    }
  }

  for (uint64_t i = 0; i < N_ARRAYS; ++i) {
    for (uint64_t j = 0; j < N_FLAT_THREADS; ++j) {
      threads[j] = std::thread(process, &arrays[i][j]);
    }
    for (uint64_t j = 0; j < N_FLAT_THREADS; ++j) {
      threads[j].join();
    }

    std::string prefix = "arrays[" + std::to_string(i) + "] processed: ";
    Output(prefix, arrays[i]);
  }

  std::cout << std::endl;

  return GetFinalResult(arrays);
}


/// THIS FAILS
uint32_t Processor::ProcessThreadsNested(uint32_t n) {
  std::cout << "START ProcessThreadsNested" << std::endl;

  uint64_t arrays[N_ARRAYS][N_VALUES];

  for (uint64_t i = 0; i < N_ARRAYS; ++i) {
    for (uint64_t j = 0; j < N_VALUES; ++j) {
      arrays[i][j] = n + i + j;
    }
  }

  std::thread threads[N_IMMEDIATE_THREADS];

  auto process = [](uint64_t* p_array) {
    std::thread nested_threads[N_NESTED_THREADS];
    for (uint64_t k = 0; k < N_NESTED_THREADS; ++k) {
      nested_threads[k] = std::thread(ProcessOneValueInplace, p_array + k);
    }
    for (uint64_t k = 0; k < N_NESTED_THREADS; ++k) {
      nested_threads[k].join();
    }
  };

  for (uint64_t i = 0; i < N_ARRAYS; ++i) {
    if (!i) {
      Output("Before processing:   ", arrays[i]);
    } else {
      Output("                     ", arrays[i]);
    }
  }

  for (uint64_t i = 0; i < N_ARRAYS; ++i) {
    for (uint64_t j = 0; j < N_IMMEDIATE_THREADS; ++j) {
      uint64_t* p_cur_array = &arrays[i][j * N_NESTED_THREADS];
      threads[j] = std::thread(process, p_cur_array);
    }
    for (uint64_t j = 0; j < N_IMMEDIATE_THREADS; ++j) {
      threads[j].join();
    }

    std::string prefix = "arrays[" + std::to_string(i) + "] processed: ";
    Output(prefix, arrays[i]);
  }

  std::cout << std::endl;

  return GetFinalResult(arrays);
}



uint32_t Processor::ProcessTGFlat(uint32_t n) {
  std::cout << "START ProcessTGFlat" << std::endl;;

  uint64_t arrays[N_ARRAYS][N_VALUES];

  for (uint64_t i = 0; i < N_ARRAYS; ++i) {
    for (uint64_t j = 0; j < N_VALUES; ++j) {
      arrays[i][j] = n + i + j;
    }
  }

  TaskGroup tg;

  for (uint64_t i = 0; i < N_ARRAYS; ++i) {
    if (!i) {
      Output("Before processing:   ", arrays[i]);
    } else {
      Output("                     ", arrays[i]);
    }
  }

  for (uint64_t i = 0; i < N_ARRAYS; ++i) {
    auto& cur_array = arrays[i];
    for (uint64_t j = 0; j < N_VALUES; ++j) {
      tg.Schedule([&cur_array, j]() {
        ProcessOneValueInplace(&cur_array[j]);
      });
    }
    tg.RunScheduled();

    std::string prefix = "arrays[" + std::to_string(i) + "] processed: ";
    Output(prefix, arrays[i]);
  }

  std::cout << std::endl;

  return GetFinalResult(arrays);
}


uint32_t Processor::ProcessTGNested(uint32_t n) {
  std::cout << "START ProcessTGNested" << std::endl;

  uint64_t arrays[N_ARRAYS][N_VALUES];

  for (uint64_t i = 0; i < N_ARRAYS; ++i) {
    for (uint64_t j = 0; j < N_VALUES; ++j) {
      arrays[i][j] = n + i + j;
    }
  }

  TaskGroup tg_immediate;

  auto process = [](uint64_t* p_array_slice) {
    TaskGroup tg_nested;
    for (uint64_t k = 0; k < N_NESTED_THREADS; ++k) {
      tg_nested.Schedule([p_array_slice, k]() {
        ProcessOneValueInplace(p_array_slice + k);
      });
    }
    tg_nested.RunScheduled();
  };

  for (uint64_t i = 0; i < N_ARRAYS; ++i) {
    if (!i) {
      Output("Before processing:   ", arrays[i]);
    } else {
      Output("                     ", arrays[i]);
    }
  }

  for (uint64_t i = 0; i < N_ARRAYS; ++i) {
    auto& cur_array = arrays[i];

    for (uint64_t j = 0; j < N_IMMEDIATE_THREADS; ++j) {
      tg_immediate.Schedule([&process, &cur_array, j]() {
        process(&cur_array[j * N_NESTED_THREADS]);
      });
    }
    tg_immediate.RunScheduled();

    std::string prefix = "arrays[" + std::to_string(i) + "] processed: ";
    Output(prefix, arrays[i]);
  }

  std::cout << std::endl;

  return GetFinalResult(arrays);
}


#include <example/example.h>

#include <utility>
#include <string>
#include <algorithm>
#include <thread>
#include <iostream>


constexpr uint64_t N_VALUES = 6;

constexpr uint64_t N_FLAT_THREADS = N_VALUES;

constexpr uint64_t N_IMMEDIATE_THREADS = 2;
constexpr uint64_t N_NESTED_THREADS = 3;

static_assert(N_IMMEDIATE_THREADS * N_NESTED_THREADS == N_VALUES,
    "check values count");
static_assert(
    N_IMMEDIATE_THREADS + N_IMMEDIATE_THREADS * N_NESTED_THREADS <= POOL_SIZE,
    "check sufficient thread pool size");


static void Output(const std::string& prefix, uint64_t p_values[N_VALUES]) {
  std::cout << prefix;
  for (uint64_t i = 0; i < N_VALUES; ++i) {
    std::cout << p_values[i] << " ";
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
static uint32_t GetFinalResult(uint64_t* p_values_0, uint64_t* p_values_1) {
  /// specific processing here is not really important
  uint64_t result = 0;
  for (uint64_t i = 0; i < N_VALUES; ++i) {
    uint64_t mx = std::max(p_values_1[i], p_values_0[i]);
    uint64_t mn = std::min(p_values_1[i], p_values_0[i]);
    result += mx - mn;
  }
  return static_cast<uint32_t>(result);
}


uint32_t Processor::ProcessNoThreads(uint32_t n) {
  std::cout << "START ProcessNoThreads" << std::endl;

  uint64_t values[2][N_VALUES];

  for (uint64_t i = 0; i < N_VALUES; ++i) {
    values[0][i] = n + i;
    values[1][i] = n + i + 1;
  }

  Output("Before processing:   ", values[0]);
  Output("                     ", values[1]);

  for (uint64_t i = 0; i < N_VALUES; ++i) {
    ProcessOneValueInplace(&values[0][i]);
  }

  Output("values[0] processed: ", values[0]);

  for (uint64_t i = 0; i < N_VALUES; ++i) {
    ProcessOneValueInplace(&values[1][i]);
  }

  Output("values[1] processed: ", values[1]);

  std::cout << std::endl;

  return GetFinalResult(values[0], values[1]);
}


uint32_t Processor::ProcessThreadsFlat(uint32_t n) {
  std::cout << "START ProcessThreadsFlat" << std::endl;;

  uint64_t values[2][N_VALUES];
  for (uint64_t i = 0; i < N_VALUES; ++i) {
    values[0][i] = n + i;
    values[1][i] = n + i + 1;
  }

  std::thread threads[N_FLAT_THREADS];

  auto process = [](uint64_t* p_value) {
    ProcessOneValueInplace(p_value);
  };

  Output("Before processing:   ", values[0]);
  Output("                     ", values[1]);

  for (uint64_t i = 0; i < N_FLAT_THREADS; ++i) {
    threads[i] = std::thread(process, &values[0][i]);
  }
  for (uint64_t i = 0; i < N_FLAT_THREADS; ++i) {
    threads[i].join();
  }

  Output("values[0] processed: ", values[0]);

  for (uint64_t i = 0; i < N_FLAT_THREADS; ++i) {
    threads[i] = std::thread(process, &values[1][i]);
  }
  for (uint64_t i = 0; i < N_FLAT_THREADS; ++i) {
    threads[i].join();
  }

  Output("values[1] processed: ", values[1]);

  std::cout << std::endl;

  return GetFinalResult(values[0], values[1]);
}


uint32_t Processor::ProcessThreadsNested(uint32_t n) {
  std::cout << "START ProcessThreadsNested" << std::endl;

  uint64_t values[2][N_VALUES];
  for (uint64_t i = 0; i < N_VALUES; ++i) {
    values[0][i] = n + i;
    values[1][i] = n + i + 1;
  }

  std::thread threads[N_IMMEDIATE_THREADS];

  auto process = [](uint64_t* p_values) {
    std::thread nested_threads[N_NESTED_THREADS];
    for (uint64_t i = 0; i < N_NESTED_THREADS; ++i) {
      nested_threads[i] = std::thread(ProcessOneValueInplace, p_values + i);
    }
    for (uint64_t i = 0; i < N_NESTED_THREADS; ++i) {
      nested_threads[i].join();
    }
  };

  Output("Before processing:   ", values[0]);
  Output("                     ", values[1]);

  for (uint64_t i = 0; i < N_IMMEDIATE_THREADS; ++i) {
    uint64_t* p_values_0 = &values[0][i * N_NESTED_THREADS];
    threads[i] = std::thread(process, p_values_0);
  }
  for (uint64_t i = 0; i < N_IMMEDIATE_THREADS; ++i) {
    threads[i].join();
  }

  Output("values[0] processed: ", values[0]);

  for (uint64_t i = 0; i < N_IMMEDIATE_THREADS; ++i) {
    uint64_t* p_values_1 = &values[1][i * N_NESTED_THREADS];
    threads[i] = std::thread(process, p_values_1);
  }
  for (uint64_t i = 0; i < N_IMMEDIATE_THREADS; ++i) {
    threads[i].join();
  }

  Output("values[1] processed: ", values[1]);

  std::cout << std::endl;

  return GetFinalResult(values[0], values[1]);
}

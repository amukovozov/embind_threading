#include <example/example.h>

#include <iostream>
#include <chrono>


int main() {
  constexpr uint64_t n = 1000000000;

  using ClockT = std::chrono::high_resolution_clock;
  using DurT = ClockT::duration;

  Processor processor;

  uint32_t result = 0;
  std::chrono::time_point<ClockT, DurT> t0;
  std::chrono::time_point<ClockT, DurT> t1;
  std::chrono::duration<double> dur_s;

  t0 = std::chrono::high_resolution_clock::now();
  result = processor.ProcessNoThreads(n);
  t1 = std::chrono::high_resolution_clock::now();
  dur_s = t1 - t0;
  std::cout << "result: " << result << ", time: " << dur_s.count() << std::endl;

  t0 = std::chrono::high_resolution_clock::now();
  result = processor.ProcessThreadsFlat(n);
  t1 = std::chrono::high_resolution_clock::now();
  dur_s = t1 - t0;
  std::cout << "result: " << result << ", time: " << dur_s.count() << std::endl;

  t0 = std::chrono::high_resolution_clock::now();
  result = processor.ProcessThreadsNested(n);
  t1 = std::chrono::high_resolution_clock::now();
  dur_s = t1 - t0;
  std::cout << "result: " << result << ", time: " << dur_s.count() << std::endl;

  t0 = std::chrono::high_resolution_clock::now();
  result = processor.ProcessTGFlat(n);
  t1 = std::chrono::high_resolution_clock::now();
  dur_s = t1 - t0;
  std::cout << "result: " << result << ", time: " << dur_s.count() << std::endl;

  t0 = std::chrono::high_resolution_clock::now();
  result = processor.ProcessTGNested(n);
  t1 = std::chrono::high_resolution_clock::now();
  dur_s = t1 - t0;
  std::cout << "result: " << result << ", time: " << dur_s.count() << std::endl;

  return 0;
}

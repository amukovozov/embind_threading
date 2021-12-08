#include <example/example.h>

#include <iostream>


int main() {
  constexpr uint64_t n = 1000000000;

  Processor processor;

  uint32_t result = 0;

  result = processor.ProcessNoThreads(n);
  std::cout << "result: " << result << std::endl;
  result = processor.ProcessThreadsFlat(n);
  std::cout << "result: " << result << std::endl;
  result = processor.ProcessThreadsNested(n);
  std::cout << "result: " << result << std::endl;
  result = processor.ProcessTGFlat(n);
  std::cout << "result: " << result << std::endl;
  result = processor.ProcessTGNested(n);
  std::cout << "result: " << result << std::endl;

  return 0;
}

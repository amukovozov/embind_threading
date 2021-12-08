#pragma once

#include <cstdint>

class Processor {
public:
  /// parameter n controls the length of the processing
  uint32_t ProcessNoThreads(uint32_t n);
  uint32_t ProcessThreadsFlat(uint32_t n);
  uint32_t ProcessThreadsNested(uint32_t n);
  uint32_t ProcessTGFlat(uint32_t n);
  uint32_t ProcessTGNested(uint32_t n);
};

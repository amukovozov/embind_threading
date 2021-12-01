#pragma once

#include <example/example.h>

#include <emscripten/bind.h>


Processor* CreateProcessor() {
  return new Processor;
}


using namespace emscripten;


EMSCRIPTEN_BINDINGS(Processor) {
  class_<Processor>("Processor")
      .constructor(&CreateProcessor, allow_raw_pointers())

      .function("ProcessNoThreads",
                &Processor::ProcessNoThreads)
      .function("ProcessThreadsFlat",
                &Processor::ProcessThreadsFlat)
      .function("ProcessThreadsNested",
                &Processor::ProcessThreadsNested)
      ;
}



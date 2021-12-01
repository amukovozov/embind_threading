project(example_wasm)

if (EMSCRIPTEN)
  add_library(embind_example INTERFACE
    include/embind_example/embind_example.h
    )
  target_include_directories(embind_example INTERFACE include)
  target_link_libraries(embind_example INTERFACE example)

  add_executable(example_wasm src/example_wasm.cpp)
  target_link_libraries(example_wasm PRIVATE embind_example)
  target_compile_options(example_wasm PRIVATE "-pthread")

  set(example_wasm_LINK_FLAGS "--bind -s WASM -s NO_EXIT_RUNTIME")

  string(APPEND example_wasm_LINK_FLAGS " --no-entry")
  string(APPEND example_wasm_LINK_FLAGS " -s EXPORT_ALL")

  string(APPEND example_wasm_LINK_FLAGS " -s NO_FILESYSTEM")
  string(APPEND example_wasm_LINK_FLAGS " -s NO_USE_SDL -s NO_USE_SDL_MIXER")

  string(APPEND example_wasm_LINK_FLAGS " -pthread")
  string(APPEND example_wasm_LINK_FLAGS
    " -s PTHREAD_POOL_SIZE=${POOL_SIZE}")
  string(APPEND example_wasm_LINK_FLAGS " -s PTHREAD_POOL_SIZE_STRICT=2")
  string(APPEND example_wasm_LINK_FLAGS " -s PTHREADS_DEBUG=1")

  string(APPEND example_wasm_LINK_FLAGS " -s ENVIRONMENT=web,worker")

  string(APPEND example_wasm_LINK_FLAGS " -s ALLOW_MEMORY_GROWTH")
  string(APPEND example_wasm_LINK_FLAGS " -s INITIAL_MEMORY=547618816")
  string(APPEND example_wasm_LINK_FLAGS " -s TOTAL_STACK=512mb")

  string(APPEND example_wasm_LINK_FLAGS " -s ASSERTIONS")
  string(APPEND example_wasm_LINK_FLAGS " -s NO_DISABLE_EXCEPTION_CATCHING")
  string(APPEND example_wasm_LINK_FLAGS " -s STRICT")
  string(APPEND example_wasm_LINK_FLAGS " -s STRICT_JS")
  string(APPEND example_wasm_LINK_FLAGS " -s NO_LEGALIZE_JS_FFI")
  string(APPEND example_wasm_LINK_FLAGS
    " -s NO_HTML5_SUPPORT_DEFERRING_USER_SENSITIVE_REQUESTS")

  set_target_properties(example_wasm
    PROPERTIES LINK_FLAGS ${example_wasm_LINK_FLAGS})
endif()

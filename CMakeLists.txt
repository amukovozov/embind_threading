cmake_minimum_required(VERSION 3.12)
project(example)

if (EMSCRIPTEN)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/artifacts/served)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/artifacts/lib_wasm)
else()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/artifacts/bin)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/artifacts/lib)
endif()

set(POOL_SIZE 12)

add_library(example
  include/example/example.h
  src/example.cpp
  )
target_include_directories(example PUBLIC include)
target_compile_definitions(example PUBLIC "-DPOOL_SIZE=${POOL_SIZE}")
target_link_libraries(example PUBLIC pthread)

add_subdirectory(wrap/embind_example)

add_subdirectory(test)

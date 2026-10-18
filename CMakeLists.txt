cmake_minimum_required(VERSION 3.16)
project(entropy_wordle_solver LANGUAGES CXX)

# This project scores many candidate/answer pairs; a Release build keeps the
# exact entropy strategy responsive on the full word list.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(wordle-solver apps/solver.cpp)
add_executable(wordle-benchmark apps/benchmark.cpp)
target_include_directories(wordle-solver PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(wordle-benchmark PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

include(CTest)
if(BUILD_TESTING)
  add_executable(wordle-tests tests/test_wordle.cpp)
  target_include_directories(wordle-tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
  add_test(NAME wordle-tests COMMAND wordle-tests)
endif()

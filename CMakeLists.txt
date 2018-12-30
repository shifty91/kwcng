cmake_minimum_required(VERSION 2.6)
project(kwcng)

set(SRCS
  src/main.cc
  src/word_counter.cc
  src/config.cc
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pedantic -Wall -march=native")
set(CMAKE_BUILD_TYPE "Release")
set(VERSION "1.0")

# check for C++17
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++17" HAVE_CPP17)
if (NOT HAVE_CPP17)
  message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER} has no C++17 support.")
endif()

# threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# config file
configure_file(
  "${PROJECT_SOURCE_DIR}/kwcng_config.in"
  "${PROJECT_BINARY_DIR}/kwcng_config.h"
  )
include_directories("${PROJECT_BINARY_DIR}")

include_directories("src")
add_executable(kwcng ${SRCS})
target_link_libraries(kwcng Threads::Threads)
install(TARGETS kwcng DESTINATION bin COMPONENT binaries)

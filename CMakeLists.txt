cmake_minimum_required(VERSION 2.6)
project(ktailng)

set(SRCS
  src/main.cc
  src/reader.cc
  src/writer.cc
  src/inotify.cc
  src/kqueue.cc
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

# functions
include(CheckFunctionExists)
check_function_exists(kqueue HAVE_KQUEUE)
check_function_exists(inotify_init HAVE_INOTIFY)

# config file
configure_file(
  "${PROJECT_SOURCE_DIR}/ktailng_config.in"
  "${PROJECT_BINARY_DIR}/ktailng_config.h"
  )
include_directories("${PROJECT_BINARY_DIR}")

# kopt
include(GNUInstallDirs)
include(ExternalProject)
externalproject_add(kopt
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/lib/kopt/"
  CMAKE_ARGS
  -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
  -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
  -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/kopt
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
add_library(kopt_lib STATIC IMPORTED)
set_target_properties(kopt_lib PROPERTIES IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/kopt/${CMAKE_INSTALL_LIBDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}kopt${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(KOPT_INCLUDE_DIR "${CMAKE_BINARY_DIR}/kopt/include/")
add_dependencies(kopt_lib kopt)

include_directories("src")
include_directories(${KOPT_INCLUDE_DIR})
add_executable(ktailng ${SRCS})
target_link_libraries(ktailng Threads::Threads)
target_link_libraries(ktailng kopt_lib)
install(TARGETS ktailng DESTINATION bin COMPONENT binaries)

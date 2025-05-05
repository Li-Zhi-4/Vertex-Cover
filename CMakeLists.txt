cmake_minimum_required(VERSION 3.10...3.26)
project(ece650-a4)

# Enable C++11 support
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall ${CMAKE_CXX_FLAGS}")

# Check for MiniSat
if (EXISTS "${CMAKE_SOURCE_DIR}/minisat")
  add_subdirectory(minisat)
  include_directories(${CMAKE_SOURCE_DIR}/minisat)
else()
  message(FATAL_ERROR "MiniSat not found. Use `git clone https://git.uwaterloo.ca/ece650-w25/minisat.git`.")
endif()

# Kissat is not built with CMake: configure it manually
set(KISSAT_INCLUDE "../kissat/src")
set(KISSAT_INCLUDE "${CMAKE_SOURCE_DIR}/kissat/src")
set(KISSAT_LIB "${CMAKE_SOURCE_DIR}/kissat/build/libkissat.a")   # Update with actual library file


if (EXISTS "${KISSAT_LIB}")
  set(HAVE_KISSAT ON)
else()
  message(WARNING "Kissat library not found at ${KISSAT_LIB}. Skipping ece650-kissat.")
  set(HAVE_KISSAT OFF)
endif()

# Option to enable AddressSanitizer
option(WITH_ASAN "Enable AddressSanitizer to detect memory faults" OFF)
if(WITH_ASAN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined")
endif()

# First executable using MiniSat
add_executable(ece650-a4 ece650-a4.cpp formatter.cpp matrix.cpp)
target_link_libraries(ece650-a4 minisat-lib-static)

# Second executable using the alternative SAT solver
if(HAVE_KISSAT)
  add_executable(ece650-a4-kissat ece650-a4-kissat.cpp formatter.cpp)
  target_include_directories(ece650-a4-kissat PRIVATE ${KISSAT_INCLUDE})
  target_link_libraries(ece650-a4-kissat PRIVATE ${KISSAT_LIB})
endif()

# Test executable (still using MiniSat)
add_executable(test test.cpp formatter.cpp matrix.cpp kissat-helper.cpp)
target_link_libraries(test minisat-lib-static)

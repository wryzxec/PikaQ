cmake_minimum_required(VERSION 3.16)

project(SPSCQueueProject CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -pthread -Wall -Wextra -pedantic")

include_directories(include)

if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
  find_package(GTest REQUIRED PATHS /opt/homebrew)
elseif(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
elseif(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
    add_compile_definitions(_WIN32_WINNT=0x0A00)
endif()


find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(spsc_tests tests/spsc_tests.cpp)
target_link_libraries(spsc_tests GTest::gtest GTest::gtest_main pthread)

add_executable(benchmarks benchmarks/benchmarks.cpp)
target_link_libraries(benchmarks pthread)

enable_testing()
add_test(NAME SPSCQueueTests COMMAND spsc_tests)

add_custom_target(clean-all COMMAND ${CMAKE_COMMAND} --build . --target clean)


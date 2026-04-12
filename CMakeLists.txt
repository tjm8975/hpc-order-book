cmake_minimum_required(VERSION 3.16)

project(OrderBookApp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto")

enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/52eb8108c5bdec04579160ae17225d66034bd723.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add subdirectories (build libraries)
add_subdirectory(OrderBook)
add_subdirectory(Benchmarking)

# Main executable
add_executable(order_book_app Main.cpp)

target_link_libraries(order_book_app
    PRIVATE
        OrderBookLib
)

# Benchmarking executable
add_executable(benchmark_app BenchmarkingMain.cpp)

target_link_libraries(benchmark_app
    PRIVATE
        OrderBookLib
        BenchmarkingLib
)

# cmake -S . -B build
# OPTIONS: -DCMAKE_BUILD_TYPE=Release
# make
# ctest
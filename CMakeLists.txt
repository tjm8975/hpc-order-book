cmake_minimum_required(VERSION 3.16)

project(OrderBookApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# cmake -DCMAKE_BUILD_TYPE=Release ..
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto")

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

# mkdir build && cd build
# cmake -DCMAKE_BUILD_TYPE=Release ..
# make
add_library(BenchmarkingLib
    src/tcBenchmarkRunner.cpp
    src/tcOrderGenerator.cpp
)

target_include_directories(BenchmarkingLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# Benchmarking depends on OrderBook
target_link_libraries(BenchmarkingLib
    PUBLIC
        OrderBookLib
)
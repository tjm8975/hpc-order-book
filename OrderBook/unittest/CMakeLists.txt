add_executable(OrderBookTests
    tcMatchingEngineUT.cpp
    tcOrderBookUT.cpp
    tcPriceLevelUT.cpp
)

target_link_libraries(OrderBookTests
    PRIVATE
        OrderBookLib
        gtest
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(OrderBookTests)
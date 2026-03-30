add_library(OrderBookLib
    src/tcMatchingEngine.cpp
    src/tcOrderBook.cpp
    src/tcOrderIntake.cpp
    src/tcPriceLevel.cpp
)

# Include headers
target_include_directories(OrderBookLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# Optional: warnings
if (MSVC)
    target_compile_options(OrderBookLib PRIVATE /W4)
else()
    target_compile_options(OrderBookLib PRIVATE -Wall -Wextra -Wpedantic)
endif()
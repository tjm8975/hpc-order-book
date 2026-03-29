cmake_minimum_required(VERSION 3.16)

# Project name
project(OrderBookApp)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(order_book_app
    src/Main.cpp
    src/tcPriceLevel.cpp
    src/tcOrderBook.cpp
    src/tcMatchingEngine.cpp
    src/tcOrderIntake.cpp
)

target_include_directories(order_book_app PRIVATE inc)

if (MSVC)
    target_compile_options(order_book_app PRIVATE /W4)
else()
    target_compile_options(order_book_app PRIVATE -Wall -Wextra -Wpedantic)
endif()
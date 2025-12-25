cmake_minimum_required(VERSION 3.15)
project(MINI_BROWSER LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Library: parsing logic
add_library(parsing_lib
    src/parsing/html_tokenizer.cpp
    src/parsing/html_parser.cpp
    src/parsing/node.cpp
)

target_include_directories(parsing_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Enable testing
enable_testing()
add_subdirectory(test)
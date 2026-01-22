cmake_minimum_required(VERSION 3.15)
project(MINI_BROWSER LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Network)

# GUI Library
add_library(gui_lib
    src/gui/main_window.cpp
    src/gui/header.cpp
    src/gui/renderer.cpp
    include/gui/main_window.h 
    include/gui/header.h
    include/gui/renderer.h
    include/gui/image_cache_manager.h
)

target_include_directories(gui_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(gui_lib PUBLIC
    Qt6::Widgets
)

target_link_libraries(gui_lib PUBLIC Qt6::Network)


add_executable(browser
    src/main.cpp
)

target_link_libraries(browser PRIVATE 
    gui_lib
    parsing_lib 
    Qt6::Widgets
)

# Library: parsing logic
add_library(parsing_lib
    src/html/html_tokenizer.cpp
    src/html/html_parser.cpp
    src/html/node.cpp
    src/css/css_parser.cpp
    src/css/cssom.cpp
    src/css/computed_style.cpp
    src/css/apply_style.cpp
    src/css/layout_tree.cpp
    src/util_functions.cpp
)

target_include_directories(parsing_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(parsing_lib PUBLIC 
    Qt6::Widgets 
    Qt6::Network
)

# Enable testing
enable_testing()
add_subdirectory(test)
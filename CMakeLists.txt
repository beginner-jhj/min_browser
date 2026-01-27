cmake_minimum_required(VERSION 3.16) # Qt6 usually recommends 3.16+
project(MINI_BROWSER LANGUAGES CXX)

# --- Global Settings ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON) # Qt's Meta-Object Compiler for Signals/Slots
set(CMAKE_AUTOUIC ON) # If you use .ui files later
set(CMAKE_AUTORCC ON) # For local resources (icons, etc.)

# --- Dependencies ---
# Grouping Qt components makes it easier to manage
find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

# --- Core Parsing Library ---
# Logic for HTML/CSS. This is the "Engine" of your browser.
add_library(parsing_lib STATIC
    src/html/html_tokenizer.cpp
    src/html/html_parser.cpp
    src/html/node.cpp
    src/css/css_parser.cpp
    src/css/cssom.cpp
    src/css/computed_style.cpp
    src/css/apply_style.cpp
    src/css/layout_tree.cpp
    src/util_functions.cpp

    include/html/html_tokenizer.h
    include/html/html_parser.h
    include/html/node.h
    include/css/css_parser.h
    include/css/cssom.h
    include/css/computed_style.h
    include/css/apply_style.h
    include/css/layout_tree.h
    include/util_functions.h
)

target_include_directories(parsing_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Network is needed for fetching, Widgets for some Qt-specific types
target_link_libraries(parsing_lib PUBLIC
    Qt6::Network
    Qt6::Widgets
)

# --- GUI Library ---
# This layer handles rendering and user interaction.
add_library(gui_lib STATIC
    src/gui/main_window.cpp
    src/gui/header.cpp
    src/gui/renderer.cpp

    include/gui/main_window.h
    include/gui/header.h
    include/gui/renderer.h
    include/gui/image_cache_manager.h
)

target_include_directories(gui_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Link internal parsing logic and Qt
target_link_libraries(gui_lib PUBLIC
    parsing_lib
    Qt6::Widgets
)

# --- Main Executable ---
add_executable(browser src/main.cpp)

target_link_libraries(browser PRIVATE
    gui_lib
    parsing_lib
    Qt6::Widgets
)

# --- Testing ---
if(BUILD_TESTING) # Wrapped in a conditional for better build control
    enable_testing()
    add_subdirectory(test)
endif()

cmake_minimum_required(VERSION 3.24)
project(Tiny.cpp LANGUAGES CXX VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(TINY_BUILD_TEST "Build test module" ON)
option(TINY_GTEST_SRC "Include Source code from local." ON)


add_library(${PROJECT_NAME} STATIC
        src/Tiny.hpp
        src/Tiny.cpp
        src/OS/File.hpp
        src/OS/File.cpp
        src/TUI/TUI.hpp
        src/TUI/TUI.cpp
        src/Events/Events.hpp
        src/Events/Events.cpp
)

if (TINY_BUILD_TEST)
    if (TINY_GTEST_SRC)
        if (EXISTS ${CMAKE_SOURCE_DIR}/test/libs/gtest/CMakeLists.txt)
            add_subdirectory(${CMAKE_SOURCE_DIR}/test/libs/gtest)
            message("-- Found Source directory: \"${CMAKE_SOURCE_DIR}/test/libs/gtest\".")
        else ()
            message("-- Downloading Google test from \"https://github.com/google/googletest/archive/refs/heads/main.zip\"...")
            message("-- Press Ctrl+C to cancel.")
            include(FetchContent)
            FetchContent_Declare(
                    googletest
                    URL https://github.com/google/googletest/archive/refs/heads/main.zip
            )
            set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
            FetchContent_MakeAvailable(googletest)
        endif ()
    else ()
        message("-- Using specified path for Google test. ")
        find_package(GTest REQUIRED)
    endif ()
    add_subdirectory(test)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/test/assets ${CMAKE_BINARY_DIR}/bin/assets
    )
endif ()


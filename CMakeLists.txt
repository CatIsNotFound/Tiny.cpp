cmake_minimum_required(VERSION 3.24)
project(Tiny LANGUAGES CXX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(TINY_BUILD_TEST "Build test module" ON)
option(TINY_GTEST_SRC "Include Source code from local." ON)
option(TINY_USE_GPM "Include gpm library for project (Only for linux)." OFF)
message("-- Tiny.cpp is configuring...")
message("-- Build Test Module: ${TINY_BUILD_TEST}")
message("-- Installation path: ${CMAKE_INSTALL_PREFIX}")
message("-- P.S: To disabled building test module, please set '-DTINY_BUILD_TEST=OFF' option.")
message("--      If your GTest submodule is already installed by package manager, please set") 
message("--      '-DTINY_GTEST_SRC=OFF' option.")

if (TINY_USE_GPM)
if (LINUX)
    find_path(GPM_INCLUDE_DIR
        NAMES gpm.h
        PATH_SUFFIXES gpm
    )

    find_library(GPM_LIBRARY
        NAMES gpm
    )

else()
    message(FATAL_ERROR "-- Error: You are not using Linux system! Please disabled the option \"TINY_USE_GPM\".")
endif()
endif()

add_library(${PROJECT_NAME} STATIC
        src/Tiny.hpp
        src/Tiny.cpp
        src/OS/File.hpp
        src/OS/File.cpp
        src/OS/System.cpp
        src/OS/System.hpp
        src/Events/Events.hpp
        src/Events/Events.cpp
        src/TUI/Terminal.hpp
        src/TUI/Terminal.cpp
        src/TUI/Terminal_Int.hpp
        src/TUI/TUI.hpp
        src/TUI/TUI.cpp
        src/Parser/CommandParser.hpp
        src/Parser/CommandParser.cpp
)

# Link Windows libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE pdh)
endif()

# Link Linux libraries
if (TINY_USE_GPM)
    if (GPM_INCLUDE_DIR AND GPM_LIBRARY)
        message("-- Found library: libgpm")
        target_link_libraries(${PROJECT_NAME} PRIVATE ${GPM_LIBRARY})
        target_include_directories(${PROJECT_NAME} PRIVATE ${GPM_INCLUDE_DIR})
        add_compile_definitions(TINY_CPP_USE_GPM=1)
    else ()
        message(FATAL_ERROR "-- Error: Can not found the libgpm package! Please install 'libgpm' package from package manager at first!")
    endif ()
endif()

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)

install(DIRECTORY src/
        DESTINATION include/${PROJECT_NAME}
        FILES_MATCHING PATTERN "*.hpp"
)

install(FILES
        LICENSE
        README.md
        README_zh.md
        DESTINATION share/${PROJECT_NAME}
)

install(EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION lib/cmake/${PROJECT_NAME}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        @ONLY
)
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION lib/cmake/${PROJECT_NAME}
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


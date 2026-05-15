cmake_minimum_required(VERSION 3.10)

enable_testing()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

function(addModule NAME MODULE_LIST)
    add_executable(${NAME}
            ${ARGN}
    )
    set(${MODULE_LIST} ${${MODULE_LIST}} ${NAME} PARENT_SCOPE)
endfunction()

addModule(TestFile MY_MODULE
        testFile.cpp
)

addModule(TestEvent MY_MODULE
        testEvent.cpp
)

addModule(TestTUI MY_MODULE_WITHOUT_GTEST
        Test_TUI.cpp
)


foreach (MODULE IN ITEMS ${MY_MODULE})
    set_target_properties(${MODULE} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )

    target_link_libraries(${MODULE}
            Tiny.cpp
            GTest::gtest_main
    )
    include(GoogleTest)
    gtest_discover_tests(${MODULE})
    add_test(${MODULE} COMMAND ${MODULE})
endforeach ()

foreach (MODULE IN ITEMS ${MY_MODULE_WITHOUT_GTEST})
    set_target_properties(${MODULE} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )

    target_link_libraries(${MODULE}
            Tiny.cpp
    )
    add_test(${MODULE} COMMAND ${MODULE})
endforeach ()


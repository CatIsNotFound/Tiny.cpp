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
        TestFile.cpp
)

addModule(TestSystem MY_MODULE
        TestSystem.cpp
)

addModule(TestDateTime MY_MODULE
        TestDateTime.cpp
)

addModule(TestEvent MY_MODULE
        TestEvent.cpp
)

addModule(TestEventMap MY_MODULE
        TestEventMap.cpp
)

addModule(TestCommandParser MY_MODULE
        TestCmdParser.cpp
)

addModule(TestTUI MY_MODULE_WITHOUT_GTEST
        Test_TUI.cpp
)

addModule(TestTempFile MY_MODULE_WITHOUT_GTEST
        Test_TempFile.cpp
)

addModule(TestInput MY_MODULE_WITHOUT_GTEST
        Test_Input.cpp
)

addModule(TestDT MY_MODULE_WITHOUT_GTEST
        Test_Dt.cpp
)

addModule(TestEV MY_MODULE_WITHOUT_GTEST
        Test_Ev.cpp
)


foreach (MODULE IN ITEMS ${MY_MODULE})
    set_target_properties(${MODULE} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/gtests
    )

    target_link_libraries(${MODULE}
            Tiny
            GTest::gtest_main
    )
    include(GoogleTest)
    gtest_discover_tests(${MODULE})
    add_test(${MODULE} COMMAND ${MODULE})
endforeach ()

foreach (MODULE IN ITEMS ${MY_MODULE_WITHOUT_GTEST})
    set_target_properties(${MODULE} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test
    )

    target_link_libraries(${MODULE}
            Tiny
    )
    add_test(${MODULE} COMMAND ${MODULE})
endforeach ()


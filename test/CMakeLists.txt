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

addModule(TestSocket MY_MODULE
        TestSocket.cpp
)

addModule(TestIniParser MY_MODULE
        TestIniParser.cpp
)

addModule(TestInput MY_MODULE_WITHOUT_GTEST
        Test_Input.cpp
)

addModule(TestNet MY_MODULE_WITHOUT_GTEST
        Test_Net.cpp
)

addModule(TestTerm MY_MODULE_WITHOUT_GTEST
        Test_Term.cpp
)

addModule(TestHostResolver MY_MODULE_WITHOUT_GTEST
        Test_HostResolver.cpp
)

addModule(TestEventBus MY_MODULE_WITHOUT_GTEST
        Test_EventBus.cpp
)

addModule(TestTUI MY_MODULE_WITHOUT_GTEST
        Test_TUI.cpp
)


foreach (MODULE IN ITEMS ${MY_MODULE})
    set_target_properties(${MODULE} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/gtests
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
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
    )

    target_link_libraries(${MODULE}
            Tiny
    )
    add_test(${MODULE} COMMAND ${MODULE})
endforeach ()


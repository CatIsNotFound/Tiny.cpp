cmake_minimum_required(VERSION 3.10)

enable_testing()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

function(addModule NAME MODULE_LIST)
    add_executable(${NAME}
            ${ARGN}
    )
    set(${MODULE_LIST} ${${MODULE_LIST}} ${NAME} PARENT_SCOPE)
endfunction()

#########################################################################################

addModule(Msgbox MY_MODULE
    Msgbox/DemoMessagebox.cpp
    Misc/Misc.hpp
)

addModule(OSInfo MY_MODULE
    OSInfo/DemoOSInfo.cpp
)


#########################################################################################

foreach (MODULE IN ITEMS ${MY_MODULE})
    set_target_properties(${MODULE} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/demos
    )

    target_link_libraries(${MODULE}
            Tiny
    )

    add_test(${MODULE} COMMAND ${MODULE})
endforeach ()
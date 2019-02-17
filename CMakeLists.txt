cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)


if(NOT WIN32)
    execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/init-submodules.sh WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" RESULT_VARIABLE INIT_SUBMODULES_RC)
    if(NOT(INIT_SUBMODULES_RC EQUAL 0))
        message(FATAL_ERROR "Submodules initialization failed with exit code ${INIT_SUBMODULES_RC}, check the output and try to run ./init-submodules.sh manually. CMake will exit.")
    endif()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/deps/cotire/CMake)
include(cotire)

include(CMakeListsFiles.cmake)

project(seminar_testing)
set(EXECUTABLE_NAME "seminar_testing")


add_library(AppLib STATIC ${APPLIB_SOURCE_FILES})
set_target_properties(AppLib PROPERTIES LINKER_LANGUAGE CXX)

add_executable(App ${APP_TYPE} ${APP_SOURCE_FILES} ${GLOBAL_HOTKEY_FILES})
set_target_properties(App PROPERTIES OUTPUT_NAME ${EXECUTABLE_NAME})

target_link_libraries(App AppLib)

find_package(Threads REQUIRED)


add_library(GoogleTestLib STATIC ${GOOGLETESTLIB_SOURCE_FILES})
target_link_libraries(GoogleTestLib AppLib)

add_executable(Tests ${APP_TESTS_TYPE} ${TESTS_SOURCE_FILES})

target_link_libraries(Tests GoogleTestLib)
target_link_libraries(Tests Threads::Threads)
target_link_libraries(Tests Threads::Threads)

enable_testing()

configure_include_dirs()

if(__COTIRE_INCLUDED)
    set_target_properties(AppLib PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
    set_target_properties(AppLib PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "src/lib/StdAfx.hpp")

    set_target_properties(App PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
    set_target_properties(App PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "src/main/StdAfxMain.hpp")

    set_target_properties(Tests PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
    set_target_properties(Tests PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "src/test/StdAfxTest.hpp")

    cotire(AppLib App Tests)
endif()

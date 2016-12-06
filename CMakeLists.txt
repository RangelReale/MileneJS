cmake_minimum_required(VERSION 2.8)
project(MileneJS)

enable_testing()

set(CMAKE_CXX_STANDARD 14)
add_definitions(-DDUK_OPT_CPP_EXCEPTIONS)

include_directories(include)
include_directories(duktape)
file(GLOB_RECURSE headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  include/*.h include/milenejs/*.h)

add_executable(example_runner ${CMAKE_CURRENT_SOURCE_DIR}/example/Test.cpp ${headers}
    ../duktape/duktape.c
    ../duktape/duktape.h
    ../duktape/duk_config.h
)

#include_directories(test)
add_executable(test_runner ${CMAKE_CURRENT_SOURCE_DIR}/test/Test.cpp ${headers}
    ${CMAKE_CURRENT_SOURCE_DIR}/test/error_tests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/exception_tests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/interop_tests.cpp

    ../duktape/duktape.c
    ../duktape/duktape.h
    ../duktape/duk_config.h
)

SET_SOURCE_FILES_PROPERTIES( ../duktape/duktape.c PROPERTIES LANGUAGE CXX )

add_test(NAME test_runner COMMAND test_runner)
cmake_minimum_required(VERSION 2.8)
project(MileneJS)

set(CMAKE_CXX_STANDARD 14)
add_definitions(-DDUK_OPT_CPP_EXCEPTIONS)

include_directories(include)
INCLUDE_DIRECTORIES(duktape)
file(GLOB_RECURSE headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  include/*.h include/milenejs/*.h)

add_executable(test_runner ${CMAKE_CURRENT_SOURCE_DIR}/test/Test.cpp ${headers}
    ../duktape/duktape.c
    ../duktape/duktape.h
    ../duktape/duk_config.h
)

SET_SOURCE_FILES_PROPERTIES( ../duktape/duktape.c PROPERTIES LANGUAGE CXX )


cmake_minimum_required(VERSION 3.14)
project(proc-rock-lib)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE PROC_ROCK_LIB_SOURCES
    include/*.h
    src/*.h
    src/*.cpp)

add_library(proc-rock-lib STATIC ${PROC_ROCK_LIB_SOURCES})
source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${PROC_ROCK_LIB_SOURCES})

target_include_directories(proc-rock-lib INTERFACE include)
target_include_directories(proc-rock-lib PRIVATE include/procrocklib)
target_include_directories(proc-rock-lib PRIVATE src)

target_compile_options(proc-rock-lib PUBLIC "$<$<BOOL:${MSVC}>:/permissive->")
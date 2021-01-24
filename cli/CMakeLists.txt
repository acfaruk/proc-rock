cmake_minimum_required(VERSION 3.14)
project(proc-rock-cli)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE PROC_ROCK_CLI_SOURCES
    src/*.h
    src/*.cpp)

add_executable(proc-rock-cli ${PROC_ROCK_CLI_SOURCES})
source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${PROC_ROCK_CLI_SOURCES})

target_link_libraries(proc-rock-cli PRIVATE proc-rock-lib)

target_compile_options(proc-rock-cli PUBLIC "$<$<BOOL:${MSVC}>:/permissive->")
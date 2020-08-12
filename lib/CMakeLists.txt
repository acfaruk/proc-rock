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


find_package(glm CONFIG REQUIRED)
target_link_libraries(proc-rock-lib PUBLIC glm)

# OPENMESH
find_library(OPENMESH_CORE_LIB_R OpenMeshCore)
find_library(OPENMESH_TOOLS_LIB_R OpenMeshTools)

find_library(OPENMESH_CORE_LIB_D OpenMeshCored)
find_library(OPENMESH_TOOLS_LIB_D OpenMeshToolsd)
target_link_libraries(proc-rock-lib PRIVATE optimized ${OPENMESH_CORE_LIB_R} ${OPENMESH_TOOLS_LIB_R})
target_link_libraries(proc-rock-lib PRIVATE debug ${OPENMESH_CORE_LIB_D} ${OPENMESH_TOOLS_LIB_D})

target_compile_options(proc-rock-lib PUBLIC "$<$<BOOL:${MSVC}>:/permissive->")

target_compile_definitions(proc-rock-lib PUBLIC _USE_MATH_DEFINES)
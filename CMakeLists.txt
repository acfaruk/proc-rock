cmake_minimum_required(VERSION 3.14)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "vcpkg toolchain file")
project(proc-rock)

add_subdirectory(external)
add_subdirectory(lib)
add_subdirectory(app)
add_subdirectory(cli)
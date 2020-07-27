cmake_minimum_required(VERSION 3.14)
project(proc-rock-app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE PROC_ROCK_APP_SOURCES
    src/*.h
    src/*.cpp)

add_executable(proc-rock ${PROC_ROCK_APP_SOURCES})
source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${PROC_ROCK_APP_SOURCES})

find_package(GLEW REQUIRED)
target_link_libraries(proc-rock PRIVATE GLEW::GLEW)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(proc-rock PRIVATE glfw)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(proc-rock PRIVATE imgui::imgui)

find_package(tinyfiledialogs CONFIG REQUIRED)
target_link_libraries(proc-rock PRIVATE tinyfiledialogs::tinyfiledialogs)
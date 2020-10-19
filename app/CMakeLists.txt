cmake_minimum_required(VERSION 3.14)
project(proc-rock-app)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE PROC_ROCK_APP_SOURCES
    src/*.h
    src/*.cpp)

file(GLOB_RECURSE PROC_ROCK_APP_SHADERS
    res/shaders/*.vert
    res/shaders/*.frag)

add_executable(proc-rock ${PROC_ROCK_APP_SOURCES} ${PROC_ROCK_APP_SHADERS})
source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${PROC_ROCK_APP_SOURCES} ${PROC_ROCK_APP_SHADERS})

find_package(GLEW REQUIRED)
target_link_libraries(proc-rock PRIVATE GLEW::GLEW)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(proc-rock PRIVATE glfw)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(proc-rock PRIVATE imgui::imgui)

find_package(tinyfiledialogs CONFIG REQUIRED)
target_link_libraries(proc-rock PRIVATE tinyfiledialogs::tinyfiledialogs)

find_package(glm CONFIG REQUIRED)
target_link_libraries(proc-rock PRIVATE glm)

target_link_libraries(proc-rock PRIVATE stb)
target_link_libraries(proc-rock PRIVATE node-editor)
target_link_libraries(proc-rock PRIVATE font-awesome)

target_link_libraries(proc-rock PRIVATE proc-rock-lib)

target_compile_options(proc-rock PUBLIC "$<$<BOOL:${MSVC}>:/permissive->")
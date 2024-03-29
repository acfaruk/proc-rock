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

target_include_directories(proc-rock-lib PUBLIC include PRIVATE include/procrocklib src)

find_package(libigl CONFIG REQUIRED)
target_link_libraries(proc-rock-lib PUBLIC igl::core igl::common)

find_package(CImg CONFIG REQUIRED)
target_link_libraries(proc-rock-lib PRIVATE CImg::CImg)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(proc-rock-lib PRIVATE nlohmann_json nlohmann_json::nlohmann_json)

find_package(CGAL CONFIG REQUIRED)
target_link_libraries(proc-rock-lib PRIVATE CGAL::CGAL)

if (UNIX)
find_library(NOISELIB noise-static)
else()
find_library(NOISELIB noise)
endif()

target_link_libraries(proc-rock-lib PRIVATE ${NOISELIB})
target_link_libraries(proc-rock-lib PRIVATE xatlas)
target_link_libraries(proc-rock-lib PRIVATE stb)

target_compile_options(proc-rock-lib PUBLIC "$<$<BOOL:${MSVC}>:/permissive->")

target_compile_definitions(proc-rock-lib PRIVATE cimg_display=0 _USE_MATH_DEFINES)
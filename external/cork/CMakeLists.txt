cmake_minimum_required(VERSION 3.14)
project(cork)

set(CMAKE_CXX_STANDARD 11)

file(
	GLOB_RECURSE cork_SRC
    "include/*.h"	
    "src/*.cpp"
    "src/*.c"
    "src/*.h"
)

add_library(cork ${cork_SRC})
target_compile_definitions(cork PRIVATE _USE_MATH_DEFINES NOMINMAX NO_TIMER REDUCED CDT_ONLY TRILIBRARY DANSI_DECLARATORS)
target_include_directories(cork PUBLIC include PRIVATE src/accel src/file_formats src/isct src/math src/mesh src/rawmesh src/util)

find_library(MPIR mpir)
find_path(MPIR_INCLUDE_PATH mpir.h)
target_link_libraries(cork ${MPIR})
target_include_directories(cork PRIVATE ${MPIR_INCLUDE_PATH})
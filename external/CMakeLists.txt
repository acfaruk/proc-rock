set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(xatlas STATIC xatlas/xatlas.cpp xatlas/xatlas.h)
target_include_directories(xatlas PUBLIC xatlas)

add_library(stb STATIC stb/stb_image.h stb/stb_image_write.h stb/impl.cpp)
target_include_directories(stb PUBLIC stb)

add_library(node-editor STATIC node-editor/imnodes.cpp node-editor/imnodes.h)
target_include_directories(node-editor PUBLIC node-editor)
find_package(imgui CONFIG REQUIRED)
target_link_libraries(node-editor PRIVATE imgui::imgui)

add_library(font-awesome INTERFACE)
target_include_directories(font-awesome INTERFACE font_awesome)

add_library(tinydir INTERFACE)
target_include_directories(tinydir INTERFACE tinydir)
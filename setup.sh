#!/bin/bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git checkout tags/2020.11-1

./bootstrap-vcpkg.sh
./vcpkg update
./vcpkg install glfw3 tinyfiledialogs imgui[opengl3-glew-binding,glfw-binding] glew glm cgal libigl libnoise cimg nlohmann-json
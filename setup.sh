#!/bin/bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git checkout tags/2020.06

./bootstrap-vcpkg.sh
./vcpkg update
./vcpkg install glfw3 tinyfiledialogs imgui glew glm libigl libnoise
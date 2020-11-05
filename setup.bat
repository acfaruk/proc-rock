git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git checkout tags/2020.11-1
call bootstrap-vcpkg.bat

SET VCPKG_DEFAULT_TRIPLET=x64-windows

vcpkg install glfw3 tinyfiledialogs imgui[opengl3-glew-binding,glfw-binding] glew glm cgal libigl libnoise cimg nlohmann-json
pause
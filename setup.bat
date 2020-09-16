git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git checkout tags/2020.07
call bootstrap-vcpkg.bat

SET VCPKG_DEFAULT_TRIPLET=x64-windows

vcpkg install glfw3 tinyfiledialogs imgui[opengl3-glew-binding,glfw-binding] glew glm libigl libnoise cimg
pause
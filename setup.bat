git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git checkout tags/2020.06
call bootstrap-vcpkg.bat

SET VCPKG_DEFAULT_TRIPLET=x64-windows

vcpkg install glfw3 tinyfiledialogs imgui glew
pause
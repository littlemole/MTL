@echo off
if not exist vcpkg (
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
cmd /C bootstrap-vcpkg.bat
cd ..
msbuild MTL.sln
)
set CMAKE_TOOLCHAIN_FILE=%~dp0vcpkg\scripts\toolchains\windows.cmake
echo CMAKE_TOOLCHAIN_FILE=%CMAKE_TOOLCHAIN_FILE%
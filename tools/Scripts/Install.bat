@echo off
pushd %~dp0
cd ..

echo ========== 1) Initializing and updating submodules
git submodule update --init --recursive

echo ========== 2) Checking for VCPKG
cmd /c vcpkg\bootstrap-vcpkg.bat

echo ========== 3) Installing dependencies...
echo ---------- 3.1) Installing entt...
vcpkg\vcpkg install entt:x64-windows-static entt:x64-windows

echo ---------- 3.2) Installing glm...
vcpkg\vcpkg install glm:x64-windows-static glm:x64-windows

echo ---------- 3.3) Installing sdl2...
vcpkg\vcpkg install sdl2:x64-windows-static sdl2:x64-windows

echo ---------- 3.4) Installing yaml-cpp...
vcpkg\vcpkg install yaml-cpp:x64-windows-static yaml-cpp:x64-windows

echo ---------- 3.5) Installing spdlog...
vcpkg\vcpkg install spdlog:x64-windows-static spdlog:x64-windows

echo ---------- 3.6) Installing imgui...
vcpkg\vcpkg install imgui[core,sdl2-binding,sdl2-renderer-binding,docking-experimental]:x64-windows-static --recurse
vcpkg\vcpkg install implot:x64-windows-static --recurse

echo ========== 4) Integrating VCPKG dependencies
vcpkg\vcpkg integrate install

echo ========== 5) Creating data folder
pushd ..\
mkdir bin\Debug\data
mkdir bin\Release\data
copy data\* bin\Debug\data\
copy data\* bin\Release\data\
popd

echo ========== Finished!
popd

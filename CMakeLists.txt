cmake_minimum_required(VERSION 3.15)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/extern/vcpkg/scripts/buildsystems/vcpkg.cmake)
set(CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(
    HFT
    VERSION 0.0.1
    LANGUAGES CXX
)
find_package(GTest CONFIG REQUIRED)
enable_testing()
add_subdirectory(tests)
add_subdirectory(src)
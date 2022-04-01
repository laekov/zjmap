cmake_minimum_required(VERSION 3.21)

project(zjmap)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(glfw)

include_directories(glfw/include)
include_directories(glad/include)

add_subdirectory(map-viewer)

include_directories(map-viewer/include)

add_subdirectory(map-core)
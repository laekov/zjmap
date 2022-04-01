cmake_minimum_required(VERSION 3.21)

include_directories(include)

add_library(map-viewer STATIC include/map-viewer.h src/map-viewer.cpp)

target_link_libraries(map-viewer PUBLIC glfw)


cmake_minimum_required(VERSION 3.21)

include_directories(include)

add_executable(map-viewer map_viewer.cpp)

target_link_libraries(map-viewer PUBLIC map-core)

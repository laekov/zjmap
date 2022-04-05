cmake_minimum_required(VERSION 3.21)

include_directories(include)

add_executable(map-viewer src/map-viewer.cpp include/glutil.h src/glutil.cpp include/map-viewer.h main.cpp)

target_link_libraries(map-viewer PUBLIC map-core glfw glad)

cmake_minimum_required(VERSION 3.10)

include_directories(include)

add_executable(project0 project0.cpp src/get-nearest-pois.cpp)

add_library(project0-lib STATIC src/get-nearest-pois.cpp)

target_link_libraries(project0 PUBLIC map-core)

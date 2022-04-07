cmake_minimum_required(VERSION 3.10)

include_directories(include)

add_executable(map-viewer src/map-viewer.cpp include/glutil.h src/glutil.cpp include/map-viewer.h main.cpp include/text-manager.h src/text-manager.cpp include/Grassland/Math/MathDecl.cpp)

target_link_libraries(map-viewer PUBLIC map-core glfw glad freetype project0-lib)

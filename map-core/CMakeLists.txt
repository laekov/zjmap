cmake_minimum_required(VERSION 3.21)

include_directories(include)

add_executable(map-core map-core.cpp include/shapefile.h src/shapefile.cpp include/csv-table.h src/csv-table.cpp include/string-convert.h src/string-convert.cpp include/util.h src/util.cpp)

target_link_libraries(map-core PUBLIC map-viewer)
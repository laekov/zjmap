cmake_minimum_required(VERSION 3.21)

include_directories(include)

add_library(map-core STATIC include/shapefile.h src/shapefile.cpp include/csv-table.h src/csv-table.cpp include/string-convert.h src/string-convert.cpp include/util.h src/util.cpp include/map-core.h include/coordinate.h src/coordinate.cpp src/map-core.cpp)

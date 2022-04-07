cmake_minimum_required(VERSION 3.10)

project(zjmap)


set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g") # Debug模式下的编译指令
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O2 -Wall -DNDEBUG") # Release模式下的编译指令


add_subdirectory(map-core)
include_directories(map-core/include)
add_subdirectory(project0)

option(BUILD_TEST "Build tests" ON)
if (BUILD_TEST)
	enable_testing()
	add_test(NAME test_proj0 WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/env COMMAND project0)
	set_tests_properties(test_proj0 PROPERTIES PASS_REGULAR_EXPRESSION "PASS")
endif()


option(BUILD_VIEWER "Build map viewer" ON)
if (BUILD_VIEWER)
	add_subdirectory(glfw)
	include_directories(glfw/include)
	include_directories(glad/include)

	add_subdirectory(glad)

	add_subdirectory(freetype)
	include_directories(freetype/include)
	add_subdirectory(map-viewer)
endif()

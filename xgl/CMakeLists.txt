project (xgl)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw-3.3.4/)
add_subdirectory(external/glad-4.6/)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_library(${PROJECT_NAME}-lib ${SOURCES})

list(APPEND INCLUDE_DIRS
	external/glad-4.6/include/
	external/glfw-3.3.4/include/
	external/glfw-3.3.4/deps/
	src/)

target_include_directories(${PROJECT_NAME}-lib PRIVATE ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}-lib glfw glad)
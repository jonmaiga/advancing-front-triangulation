project(unittest)

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
	cmake_policy(SET CMP0135 NEW)
endif()

set(gtest_disable_pthreads ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

if (MSVC)
	# For Windows: Prevent overriding the parent project's compiler/linker settings
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} afront game-client-lib xgl-lib GTest::gtest_main)
target_include_directories(${PROJECT_NAME} PRIVATE 
	../xgl/external/glad-4.6/include/
	../xgl/external/glfw-3.3.4/include/
	../xgl/external/glfw-3.3.4/deps/
	../xgl/src/
	../afront/src 
	../game-client/src 
	../core/src/ 
	src/)

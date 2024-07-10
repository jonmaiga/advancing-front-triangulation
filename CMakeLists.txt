cmake_minimum_required (VERSION 3.15)
project (triangulator)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	message("Configuration type: ${CMAKE_CONFIGURATION_TYPES}")
	set(USE_MSVC_RUNTIME_LIBRARY_DLL 1) #glfw
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
else()
	#link_directories(/usr/local/lib/)
endif()

if (NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Release" 
	#AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo"
	)
	set(IS_DEVELOPMENT TRUE)
endif()

add_subdirectory(xgl)
add_subdirectory(core)
add_subdirectory(afront)
add_subdirectory(game-client)
add_subdirectory(unittest)

if ("${IS_DEVELOPMENT}")
	message("Development mode.")
	target_compile_definitions(xgl-lib PUBLIC DEVELOPMENT=1)
	target_compile_definitions(core PRIVATE DEVELOPMENT=1)
	target_compile_definitions(afront PRIVATE DEVELOPMENT=1)
	target_compile_definitions(game-client PRIVATE DEVELOPMENT=1)
	target_compile_definitions(unittest PRIVATE DEVELOPMENT=1)
endif()

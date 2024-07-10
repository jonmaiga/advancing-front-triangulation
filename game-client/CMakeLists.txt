project(game-client)
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})
add_library(${PROJECT_NAME}-lib STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE afront xgl-lib glfw glad)
set(GAME_INCLUDE_DIRS 
	../xgl/external/glad-4.6/include/
	../xgl/external/glfw-3.3.4/include/
	../xgl/external/glfw-3.3.4/deps/
	../xgl/src/
	../afront/src/
	../core/src/
	src/
)

target_include_directories(${PROJECT_NAME} PRIVATE ${GAME_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME}-lib PRIVATE ${GAME_INCLUDE_DIRS})

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}"/game-client/data/" $<TARGET_FILE_DIR:${PROJECT_NAME}>/data/)

if (MSVC)
	#removes the console
	#set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")
endif()

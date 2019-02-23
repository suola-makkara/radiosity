cmake_minimum_required(VERSION 3.10.3)
project(Radiosity)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW_BUILD_EXAMPLES FALSE CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS FALSE CACHE BOOL "" FORCE)

add_subdirectory(glfw)

set(LIB
	glfw
)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(LIB ${LIB} stdc++fs)
endif()

set(SRC
	main_temp.cpp
	gui.cpp
	scene.cpp
	plane.cpp
	shader.cpp
	camera.cpp
	render_engine.cpp
	editor.cpp
	settings.cpp
	#render_settings_temp.cpp
	#matrix_operations.cpp
# glad
	glad/src/glad.c
# imgui
	imgui/imgui.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_impl_glfw.cpp
	imgui/imgui_impl_opengl3.cpp
	imgui/imgui_widgets.cpp
	imgui/imgui_demo.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/glfw/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/glad/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/glm)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/imgui)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} ${LIB})

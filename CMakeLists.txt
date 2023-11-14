cmake_minimum_required(VERSION 3.26)
set(TARGET_NAME learn_opengl)
project(${TARGET_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

set(SOURCES_INCLUDES src/include)

add_executable(${TARGET_NAME}
        src/main.cpp
        dependencies/GLFW/include/GLFW/glfw3.h
        dependencies/GLEW/include/GLEW/glew.h)

# GLFW
target_link_libraries(${TARGET_NAME} ${CMAKE_SOURCE_DIR}/dependencies/GLFW/libglfw3.a)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/dependencies/GLFW/include)

# GLEW
target_link_libraries(${TARGET_NAME} ${CMAKE_SOURCE_DIR}/dependencies/GLEW/glew32s.lib)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/dependencies/GLEW/include)

add_compile_definitions(GLEW_STATIC)

find_package(OpenGL REQUIRED)
target_link_libraries(${TARGET_NAME} OpenGL::GL)
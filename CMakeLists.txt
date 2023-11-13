cmake_minimum_required(VERSION 3.26)
project(learn_opengl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

set(SOURCES src/main.cpp dependencies/GLFW/include/GLFW/glfw3.h)

add_executable(learn_opengl ${SOURCES})

target_link_libraries(learn_opengl ${CMAKE_SOURCE_DIR}/dependencies/GLFW/libglfw3.a)
target_include_directories(learn_opengl PUBLIC ${CMAKE_SOURCE_DIR}/dependencies/GLFW/include)

find_package(OpenGL REQUIRED)
target_link_libraries(learn_opengl OpenGL::GL)
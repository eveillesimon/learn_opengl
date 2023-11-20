cmake_minimum_required(VERSION 3.26)
set(TARGET_NAME learn_opengl)
project(${TARGET_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

set(SOURCES_INCLUDES src/include)

add_executable(${TARGET_NAME}
        src/main.cpp
        dependencies/GLFW/include/GLFW/glfw3.h
        dependencies/GLEW/include/GLEW/glew.h)

add_custom_target(resources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/res
        ${PROJECT_BINARY_DIR}/res
        COMMENT "Copying resources into binary directory")

add_dependencies(${TARGET_NAME} resources)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# GLFW
target_link_libraries(${TARGET_NAME} ${CMAKE_SOURCE_DIR}/dependencies/GLFW/libglfw3.a)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/dependencies/GLFW/include)

# GLEW
target_link_libraries(${TARGET_NAME} ${CMAKE_SOURCE_DIR}/dependencies/GLEW/glew32s.lib)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/dependencies/GLEW/include)

add_compile_definitions(GLEW_STATIC)

find_package(OpenGL REQUIRED)
target_link_libraries(${TARGET_NAME} OpenGL::GL)
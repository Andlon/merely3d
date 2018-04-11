cmake_minimum_required(VERSION 3.1)

set (CMAKE_CXX_STANDARD 11)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/embed_shaders.cmake)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/extern/glfw-3.2.1")

find_package(OpenGL REQUIRED)
find_package(Eigen3 3 REQUIRED NO_MODULE)

# Configure GLAD, which handles OpenGL extensions for us
set(GLAD_FILES
    extern/glad/src/glad.c
    extern/glad/include/glad/glad.h
    extern/glad/include/KHR/khrplatform.h)

add_library(glad STATIC ${GLAD_FILES})
target_include_directories(glad SYSTEM PUBLIC extern/glad/include)

set(CONFIGURED_DIR "${CMAKE_CURRENT_BINARY_DIR}/configured")
set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
embed_shaders(${SHADER_DIR} src/shaders.hpp.in ${CONFIGURED_DIR}/shaders.hpp)

set(LIB_HEADERS
    include/merely3d/window.hpp
    include/merely3d/frame.hpp
    include/merely3d/types.hpp
    include/merely3d/material.hpp
    include/merely3d/camera.hpp
    include/merely3d/primitives.hpp
    include/merely3d/events.hpp
    include/merely3d/color.hpp
    include/merely3d/camera_controller.hpp
    include/merely3d/app.hpp)

set(LIB_FILES
    src/window.cpp
    src/frame.cpp
    src/shader.hpp
    src/shader.cpp
    src/command_buffer.hpp
    src/renderer.hpp
    src/renderer.cpp
    src/event_convert.hpp
    src/gl_primitive.hpp
    src/app.cpp)


add_library(merely3d ${LIB_FILES} ${LIB_HEADERS})

target_include_directories(merely3d PUBLIC include)
target_include_directories(merely3d PRIVATE ${CONFIGURED_DIR})
target_include_directories(merely3d SYSTEM PRIVATE ${OPENGL_INCLUDE_DIR})
target_link_libraries(merely3d glad glfw ${OPENGL_gl_LIBRARY})
target_link_libraries(merely3d Eigen3::Eigen)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
cmake_minimum_required(VERSION 3.0)

include(${CMAKE_SOURCE_DIR}/cmake/embed_shaders.cmake)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("extern/glfw-3.2.1")

find_package(OpenGL REQUIRED)
find_package(Eigen3 3 REQUIRED NO_MODULE)

# Configure GLAD, which handles OpenGL extensions for us
set(GLAD_FILES
    extern/glad/src/glad.c
    extern/glad/include/glad/glad.h
    extern/glad/include/KHR/khrplatform.h)

add_library(glad STATIC ${GLAD_FILES})
target_include_directories(glad SYSTEM PUBLIC extern/glad/include)

set(CONFIGURED_DIR "${CMAKE_BINARY_DIR}/configured")
set(SHADER_DIR "${CMAKE_SOURCE_DIR}/shaders")
embed_shaders(${SHADER_DIR} src/shaders.hpp.in ${CONFIGURED_DIR}/shaders.hpp)

set(LIB_HEADERS
    include/merely3d/window.hpp
    include/merely3d/frame.hpp
    include/merely3d/types.hpp
    include/merely3d/material.hpp
    include/merely3d/camera.hpp
    include/merely3d/primitives.hpp
    include/merely3d/key_event.hpp
    include/merely3d/color.hpp)

set(LIB_FILES
    src/main.cpp
    src/window.cpp
    src/frame.cpp
    src/shader.hpp
    src/shader.cpp
    src/command_buffer.hpp
    src/renderer.hpp
    src/renderer.cpp
    src/key_convert.hpp)

add_executable(merely ${LIB_FILES} ${LIB_HEADERS})

target_include_directories(merely SYSTEM PRIVATE ${OPENGL_INCLUDE_DIR})
target_include_directories(merely SYSTEM PUBLIC include)
target_include_directories(merely PRIVATE ${CONFIGURED_DIR})
target_link_libraries(merely glad glfw ${OPENGL_gl_LIBRARY})
target_link_libraries(merely Eigen3::Eigen)

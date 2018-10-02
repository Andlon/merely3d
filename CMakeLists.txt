cmake_minimum_required(VERSION 3.1)

set (CMAKE_CXX_STANDARD 11)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/embed_shaders.cmake)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/extern/glfw")

find_package(OpenGL REQUIRED)
find_package(Eigen3 3.3 NO_MODULE)

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
    include/merely3d/renderable.hpp
    include/merely3d/frame.hpp
    include/merely3d/types.hpp
    include/merely3d/material.hpp
    include/merely3d/camera.hpp
    include/merely3d/primitives.hpp
    include/merely3d/events.hpp
    include/merely3d/color.hpp
    include/merely3d/camera_controller.hpp
    include/merely3d/app.hpp
	include/merely3d/mesh.hpp)

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
    src/gl_triangle_mesh.hpp
    src/gl_gc.hpp
    src/gl_gc.cpp
    src/gl_errors.hpp
    src/gl_errors.cpp
    src/app.cpp
    src/mesh_util.hpp
    src/gl_line.hpp
    src/renderers.hpp
    src/renderers.cpp
    src/shader_collection.hpp
    src/shader_collection.cpp)

if (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    add_compile_options(-Werror -Wall -Wextra)

    if (MERELY_SANITIZE)
        add_compile_options(-fsanitize=address -fsanitize=undefined)
        link_libraries(-fsanitize=address -fsanitize=undefined)
    endif()
endif()

add_library(merely3d ${LIB_FILES} ${LIB_HEADERS})
set_target_properties(merely3d PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

target_link_libraries(merely3d glad glfw ${OPENGL_gl_LIBRARY})
target_include_directories(merely3d PUBLIC include)
target_include_directories(merely3d PRIVATE ${CONFIGURED_DIR})
target_include_directories(merely3d SYSTEM PRIVATE ${OPENGL_INCLUDE_DIR})

# This shenanigans is required because when using find_package(Eigen3), there seems
# to be no proper way to override with a source-only Eigen installation, as the
# global installed configuration always takes precedence (No, this is not fixed by ${Eigen3_DIR}).
if (TARGET Eigen3::Eigen)
	target_link_libraries(merely3d Eigen3::Eigen)
elseif(DEFINED EIGEN3_ROOT OR DEFINED EIGEN3_ROOT_DIR)
	message("-- An Eigen installation was not found, but a root directory for Eigen has been configured.\n"
			"-- Attempting to use an Eigen installation from the given root directory.")
endif()

# Add the following include directories as a means of letting
# users override the Eigen version detected by find_package
target_include_directories(merely3d SYSTEM PUBLIC ${EIGEN3_ROOT} ${EIGEN3_ROOT_DIR})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)

set(TEST_FILES
    test/testmain.cpp
    test/mesh_utils.cpp)

add_executable(tests ${TEST_FILES})
target_link_libraries(tests merely3d)
target_include_directories(tests PRIVATE src extern/catch)

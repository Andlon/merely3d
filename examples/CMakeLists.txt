cmake_minimum_required(VERSION 3.1)

add_executable(merely3d_demo demo/main.cpp demo/example_model.hpp)
target_link_libraries(merely3d_demo merely3d)

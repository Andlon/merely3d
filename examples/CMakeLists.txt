cmake_minimum_required(VERSION 3.1)

add_executable(merely3d_demo demo/main.cpp demo/example_model.hpp)
target_link_libraries(merely3d_demo merely3d)

add_executable(merely3d_minimal minimal/main.cpp)
target_link_libraries(merely3d_minimal merely3d)

add_executable(merely3d_particles particles/main.cpp)
target_link_libraries(merely3d_particles merely3d)

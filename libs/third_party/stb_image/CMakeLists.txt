cmake_minimum_required(VERSION 3.10.2)
project(stb_image VERSION 1.0 LANGUAGES CXX C)

add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

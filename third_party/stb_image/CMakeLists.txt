cmake_minimum_required(VERSION 3.25.2)
project(stb_image VERSION 1.0 LANGUAGES CXX C)

add_library(stb_image STATIC src/stb_image_impl.cpp)
target_include_directories(stb_image PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

cmake_minimum_required(VERSION 3.25.2)
project(STB_IMAGE VERSION 1.0 LANGUAGES CXX C)

add_library(STB_IMAGE STATIC src/stb_image_impl.cpp)
target_include_directories(STB_IMAGE PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

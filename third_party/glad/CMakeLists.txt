cmake_minimum_required(VERSION 3.25.0)
project(glad VERSION 1.0 LANGUAGES CXX C)
add_library(glad STATIC
    src/glad.c    
)

target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

cmake_minimum_required(VERSION 3.25.0)
project(GLAD VERSION 1.0 LANGUAGES CXX C)
add_library(GLAD STATIC
    src/glad.c    
)

target_include_directories(GLAD PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

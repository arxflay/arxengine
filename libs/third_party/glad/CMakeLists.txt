cmake_minimum_required(VERSION 3.10.2)
project(glad VERSION 0.1.36 LANGUAGES C)
add_library(glad STATIC
    src/glad.c 
)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:C,ARMClang,AppleClang,Clang,GNU,LC>")
target_compile_options(glad PUBLIC "$<${gcc_like_cxx}:-fPIC>")

target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

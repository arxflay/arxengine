cmake_minimum_required(VERSION 3.10.2)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ProjectVersion.cmake)
project(arxengine VERSION ${VER_MAJOR}.${VER_MINOR} LANGUAGES C CXX)

option(ARX_LIB_STATIC "Build as static library" OFF)
option(ARX_BUILD_TEST "Build testapi" OFF)

set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

list(APPEND SOURCES
    ${SOURCE_PATH}/color.cpp
    ${SOURCE_PATH}/utils/stringutils.cpp
    ${SOURCE_PATH}/ArxException.cpp
    ${SOURCE_PATH}/ArxWindow.cpp
    ${SOURCE_PATH}/UIObject.cpp
    ${SOURCE_PATH}/ArxEngine.cpp
    ${SOURCE_PATH}/UIApp.cpp
    ${SOURCE_PATH}/Event.cpp
    ${SOURCE_PATH}/EventLoop.cpp
)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
set(OpenGL_GL_PREFERENCE LEGACY)

if (${ARX_LIB_STATIC})
    message(STATUS "Static library")
    add_library(arxengine STATIC ${SOURCES})
else()
    message(STATUS "Shared library")
    add_library(arxengine SHARED ${SOURCES})
endif()

target_compile_options(arxengine PRIVATE "$<${gcc_like_cxx}:-Wall;-Wextra;-Wconversion;-std=c++17>"  
                                                        "$<${msvc_cxx}:/W3;/std:c++17>")
target_include_directories(arxengine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
include(${CMAKE_CURRENT_SOURCE_DIR}/libs/ProjectLibs.cmake)

if (WIN32)
    target_link_libraries(arxengine PUBLIC glm glfw stb_image glad OpenGL::GL openal freetype)
else()
    target_link_libraries(arxengine PUBLIC glm glfw stb_image glad OpenGL::GL openal freetype m pthread)
endif()

if (${ARX_BUILD_TEST})
    enable_testing()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/testapi)
endif()


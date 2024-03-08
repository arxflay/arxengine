cmake_minimum_required(VERSION 3.10.2)
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/ProjectVersion.cmake")
project(arxengine VERSION ${VER_MAJOR}.${VER_MINOR} LANGUAGES C CXX)

option(ARX_LIB_STATIC "Build as static library" OFF)
option(ARX_BUILD_TEST "Build testapi" OFF)

set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")

list(APPEND SOURCES
    ${SOURCE_PATH}/media/Image.cpp
    ${SOURCE_PATH}/misc/Utils.cpp
    ${SOURCE_PATH}/media/Sound.cpp
    ${SOURCE_PATH}/internal/WavLoader.cpp
    ${SOURCE_PATH}/ArxException.cpp
    ${SOURCE_PATH}/logging/Logger.cpp
    ${SOURCE_PATH}/logging/FileLogger.cpp
    ${SOURCE_PATH}/media/SoundPlayer.cpp
    ${SOURCE_PATH}/GameApp.cpp
    ${SOURCE_PATH}/internal/UniversalExceptionHandler.cpp
    ${SOURCE_PATH}/media/Font.cpp
    ${SOURCE_PATH}/evt/Event.cpp
    ${SOURCE_PATH}/evt/EventProcessor.cpp
    ${SOURCE_PATH}/ui/Color.cpp
    ${SOURCE_PATH}/ui/UIControl.cpp
    ${SOURCE_PATH}/ui/ArxWindow.cpp
    ${SOURCE_PATH}/ArxObject.cpp
    ${SOURCE_PATH}/evt/UIEventProcessor.cpp
    ${SOURCE_PATH}/Timer.cpp
    ${SOURCE_PATH}/internal/gl/ClippingArea.cpp
    ${SOURCE_PATH}/internal/gl/Shader.cpp
    ${SOURCE_PATH}/internal/gl/VBO.cpp
    ${SOURCE_PATH}/internal/gl/VAO.cpp
    ${SOURCE_PATH}/ui/Painter.cpp
    ${SOURCE_PATH}/ui/internal/UICache.cpp
    ${SOURCE_PATH}/ui/Brush.cpp
    ${SOURCE_PATH}/gl/Texture.cpp
    ${SOURCE_PATH}/gl/Texture2D.cpp
    ${SOURCE_PATH}/internal/al/SoundDevice.cpp
    ${SOURCE_PATH}/internal/ft/FontLibrary.cpp
    ${SOURCE_PATH}/ui/FontCache.cpp
    ${SOURCE_PATH}/ui/Pen.cpp
    ${SOURCE_PATH}/ui/UIObject.cpp
    ${SOURCE_PATH}/ui/KeyEvent.cpp
)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
set(OpenGL_GL_PREFERENCE LEGACY)

if (${ARX_LIB_STATIC})
    message(STATUS "Static library")
    add_library(arxengine STATIC "${SOURCES}")
else()
    message(STATUS "Shared library")
    add_library(arxengine SHARED "${SOURCES}")
endif()

target_compile_options(arxengine PRIVATE "$<${gcc_like_cxx}:-Wall;-Wextra;-Wconversion;-std=c++17>"  
                                                        "$<${msvc_cxx}:/W3;/std:c++17>")

include("${CMAKE_CURRENT_SOURCE_DIR}/libs/ProjectLibs.cmake")

if (WIN32)
    target_link_libraries(arxengine PRIVATE glfw stb_image openal Freetype::Freetype)
    target_link_libraries(arxengine PUBLIC glm glad OpenGL::GL)
else()
    target_link_libraries(arxengine PRIVATE glfw stb_image openal Freetype::Freetype)
    target_link_libraries(arxengine PUBLIC glm glad OpenGL::GL openal m pthread)
endif()

if (${ARX_BUILD_TEST})
    enable_testing()
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/testapi")
endif()
add_definitions(-DGLFW_INCLUDE_NONE)
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/ArxBuild.cmake")
arx_set_headers(TARGET arxengine INCLUDE_DIR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include" PRIVATE_FOLDER_NAME internal)

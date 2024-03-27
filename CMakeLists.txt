cmake_minimum_required(VERSION 3.0...3.10.2)
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/ProjectVersion.cmake")
project(arxengine VERSION ${VER_MAJOR}.${VER_MINOR} LANGUAGES CXX)

option(ARX_BUILD_TEST "Build testapi" OFF)

set(ARX_SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")

list(APPEND ARX_SOURCES
    ${ARX_SOURCE_PATH}/media/Image.cpp
    ${ARX_SOURCE_PATH}/misc/Utils.cpp
    ${ARX_SOURCE_PATH}/media/Sound.cpp
    ${ARX_SOURCE_PATH}/internal/WavLoader.cpp
    ${ARX_SOURCE_PATH}/ArxException.cpp
    ${ARX_SOURCE_PATH}/logging/Logger.cpp
    ${ARX_SOURCE_PATH}/logging/FileLogger.cpp
    ${ARX_SOURCE_PATH}/media/SoundPlayer.cpp
    ${ARX_SOURCE_PATH}/GameApp.cpp
    ${ARX_SOURCE_PATH}/internal/UniversalExceptionHandler.cpp
    ${ARX_SOURCE_PATH}/media/Font.cpp
    ${ARX_SOURCE_PATH}/evt/Event.cpp
    ${ARX_SOURCE_PATH}/evt/EventProcessor.cpp
    ${ARX_SOURCE_PATH}/ui/Color.cpp
    ${ARX_SOURCE_PATH}/ui/UIControl.cpp
    ${ARX_SOURCE_PATH}/ui/ArxWindow.cpp
    ${ARX_SOURCE_PATH}/ArxObject.cpp
    ${ARX_SOURCE_PATH}/evt/UIEventProcessor.cpp
    ${ARX_SOURCE_PATH}/Timer.cpp
    ${ARX_SOURCE_PATH}/internal/gl/ClippingArea.cpp
    ${ARX_SOURCE_PATH}/internal/gl/Shader.cpp
    ${ARX_SOURCE_PATH}/internal/gl/VBO.cpp
    ${ARX_SOURCE_PATH}/internal/gl/VAO.cpp
    ${ARX_SOURCE_PATH}/ui/Painter.cpp
    ${ARX_SOURCE_PATH}/ui/internal/UICache.cpp
    ${ARX_SOURCE_PATH}/ui/Brush.cpp
    ${ARX_SOURCE_PATH}/gl/Texture.cpp
    ${ARX_SOURCE_PATH}/gl/Texture2D.cpp
    ${ARX_SOURCE_PATH}/internal/al/SoundDevice.cpp
    ${ARX_SOURCE_PATH}/internal/ft/FontLoader.cpp
    ${ARX_SOURCE_PATH}/ui/FontCache.cpp
    ${ARX_SOURCE_PATH}/ui/Pen.cpp
    ${ARX_SOURCE_PATH}/ui/UIObject.cpp
    ${ARX_SOURCE_PATH}/ui/KeyEvent.cpp
    ${ARX_SOURCE_PATH}/ui/ImageControl.cpp
    ${ARX_SOURCE_PATH}/ui/MouseEvent.cpp
    ${ARX_SOURCE_PATH}/ui/Label.cpp
    ${ARX_SOURCE_PATH}/ui/BitmapButton.cpp
    ${ARX_SOURCE_PATH}/Json.cpp
    ${ARX_SOURCE_PATH}/internal/json/DefaultJsonLexer.cpp
    ${ARX_SOURCE_PATH}/internal/json/DefaultJsonParser.cpp
    ${ARX_SOURCE_PATH}/internal/json/DefaultJsonConvertor.cpp

)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
set(OpenGL_GL_PREFERENCE LEGACY)

if (NOT BUILD_SHARED_LIBS)
    option(BUILD_SHARED_LIBS "Build as static library" OFF)
endif()

if (NOT BUILD_SHARED_LIBS OR "${BUILD_SHARED_LIBS}" STREQUAL "OFF")
    message(STATUS "Build arxengine as static library")
    add_library(arxengine STATIC "${ARX_SOURCES}")
else()
    message(STATUS "Build arxengine as shared library")
    add_library(arxengine SHARED "${ARX_SOURCES}")
    if(WIN32)
        target_compile_definitions(arxengine PUBLIC ARX_SHARED)
    endif()
endif()

target_compile_options(arxengine PRIVATE "$<${gcc_like_cxx}:-Wall;-Wextra;-Wconversion;-std=c++17;-fPIC>"  
                                                        "$<${msvc_cxx}:/W3;/wd4251;/wd4996;/std:c++17>")

target_compile_options(arxengine INTERFACE "$<${msvc_cxx}:/wd4251;/wd4996>")

include("${CMAKE_CURRENT_SOURCE_DIR}/libs/ProjectLibs.cmake")

if (WIN32)
    target_link_libraries(arxengine PRIVATE glfw stb_image OpenAL freetype glad OpenGL::GL)
    target_link_libraries(arxengine PUBLIC glm::glm)
else()
    target_link_libraries(arxengine PRIVATE glfw stb_image OpenAL::OpenAL Freetype::Freetype glad OpenGL::GL)
    target_link_libraries(arxengine PUBLIC glm::glm m pthread)
endif()

if (${ARX_BUILD_TEST})
    enable_testing()
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/testapi")
endif()

add_definitions(-DGLFW_INCLUDE_NONE)
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/ArxBuild.cmake")
arx_set_headers(TARGET arxengine INCLUDE_DIR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include" PRIVATE_FOLDER_NAME internal)

include(GNUInstallDirs)

if(WIN32)
    set(CPACK_GENERATOR ZIP)
else()
    set(CPACK_GENERATOR TGZ)
endif()

if (WIN32)
    target_sources(arxengine PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/arxengine.rc"
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/resource.h")
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set(CPACK_PACKAGE_NAME arxengine-dev)
    set(CPACK_DEBIAN_PACKAGE_CONFLICTS arxengine)
else()
    set(CPACK_PACKAGE_NAME arxengine)
    set(CPACK_DEBIAN_PACKAGE_CONFLICTS arxengine-dev)
endif()

set(CPACK_PACKAGE_VENDOR arxflay)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)
set(CPACK_PACKAGE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CPACK_PACKAGE_VERSION_MAJOR ${VER_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VER_MINOR})
set(CPACK_PACKAGE_VERSION ${VER_MAJOR}.${VER_MINOR})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PKG_VER_NAME}")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
set(CPACK_DEBIAN_PACKAGE_SOURCE "${CPACK_PACKAGE_NAME} (${CMAKE_PACKAGE_VERSION})")
set(CPACK_DEBIAN_PACKAGE_PROVIDES ${CPACK_PACKAGE_NAME})
set(CPACK_DEBIAN_PACKAGE_SECTION libs)
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "arxengine - graphical library for creating 2d games")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Alexej Fedorenko(arxflay)")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "standard")

set_target_properties(arxengine PROPERTIES VERSION ${VER_MAJOR}.${VER_MINOR} SOVERSION ${VER_MAJOR})
set_target_properties(arxengine PROPERTIES DEBUG_POSTFIX "d")

if (NOT ARX_DISABLE_INSTALL)
    install(TARGETS arxengine)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/arx_header_cache/arxengine_public_headers/arxengine DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    if (UNIX) #set default prefix as usr
        set(CMAKE_INSTALL_PREFIX "/usr")
    endif()
endif()

#this must be a last entry in this file
include(CPack)

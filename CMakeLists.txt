cmake_minimum_required(VERSION 3.10.2)
project(arxengine VERSION 0.1 LANGUAGES C CXX)

list(APPEND SOURCES
   src/color.cpp 
)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
set(OpenGL_GL_PREFERENCE LEGACY)

if (DEFINED PROJECT_TYPE AND ${PROJECT_TYPE} STREQUAL "Static")
    message(STATUS "Static library")
    add_library(arxengine STATIC ${SOURCES})
else()
    message(STATUS "Shared library")
    add_library(arxengine SHARED ${SOURCES})
endif()

target_compile_options(arxengine PRIVATE "$<${gcc_like_cxx}:-Wall;-Wextra;-Wconversion;-std=c++17>"  
                                                        "$<${msvc_cxx}:/W3;/std:c++17>")
target_include_directories(arxengine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CUSTOM_MODULES_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CUSTOM_MODULES_PATH})

find_package(OpenGL REQUIRED)
set(THIRDPARTY_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
if (WIN32)
    add_subdirectory(${THIRDPARTY_FOLDER}/glm)
    add_subdirectory(${THIRDPARTY_FOLDER}/glfw3)
else()
    find_package(GLM REQUIRED)
    find_package(GLFW3 REQUIRED)
endif()

add_subdirectory(${THIRDPARTY_FOLDER}/glad)
add_subdirectory(${THIRDPARTY_FOLDER}/stb_image)

if (WIN32)
    target_link_libraries(arxengine PRIVATE ${GLM} ${GLFW3} stb_image glad ${OpenGL})
else()
    target_link_libraries(arxengine PRIVATE ${GLM} ${GLFW3} stb_image glad ${OpenGL} m pthread )
endif()

if (DEFINED BUILD_TEST AND ${BUILD_TEST})
    enable_testing()
    add_subdirectory(testapi)
    add_test(TestApi testapi) 
endif()


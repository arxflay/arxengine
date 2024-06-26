cmake_minimum_required(VERSION 3.0...3.10.2)
project(testapi VERSION 0.1.36 LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(SOURCE_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(TEST_DATA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/testdata")

list(APPEND TEST_SOURCES
    ${SOURCE_FOLDER}/main.cpp
    ${SOURCE_FOLDER}/base64tests.cpp
)

add_executable(testapi ${TEST_SOURCES})
target_link_libraries(testapi PRIVATE arxengine GTest::gtest GTest::gtest_main)
target_compile_options(testapi PRIVATE "$<${gcc_like_cxx}:-Wall;-Wextra;-Wconversion;-std=c++17>"  
                                                        "$<${msvc_cxx}:/W3;/std:c++17>")

if(WIN32 AND ${BUILD_SHARED_LIBS} STREQUAL "ON")
    add_custom_command(TARGET testapi POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        $<TARGET_FILE:arxengine>
        $<TARGET_FILE_DIR:testapi>)
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/testdefs.h.in" testdefs.h @ONLY)
target_include_directories(testapi PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

include(GoogleTest)
gtest_add_tests(TARGET testapi SOURCES "${TEST_SOURCES}")

cmake_minimum_required(VERSION 3.10.2)
project(testapi VERSION 1.0 LANGUAGES C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(SOURCE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/src)

list(APPEND TEST_SOURCES
    ${SOURCE_FOLDER}/main.cpp
)

add_executable(testapi ${TEST_SOURCES})
target_link_libraries(testapi PRIVATE GTest::gtest GTest::gtest_main)
target_link_libraries(testapi PUBLIC arxengine)
target_compile_options(testapi PRIVATE "$<${gcc_like_cxx}:-Wall;-Wextra;-Wconversion;-std=c++17>"  
                                                        "$<${msvc_cxx}:/W3;/std:c++17>")

find_package(OpenGL REQUIRED)

if(NOT WIN32)
    set(CPM_LOCAL_PACKAGES_ONLY True)
endif()

set(CPM_SOURCE_CACHE ${CMAKE_CURRENT_LIST_DIR}/third_party/CPM)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchCPM.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM/CPM.cmake)
CPMAddPackage("gh:g-truc/glm#0.9.9.8")
CPMAddPackage(NAME glfw3
    GITHUB_REPOSITORY glfw/glfw
    VERSION 3.3.1
    GIT_TAG ecda86fa4f89ecdd364e5a1a22645030fe0ced6e
    OPTIONS
        "GLFW_BUILD_TESTS OFF"
        "GLFW_BUILD_EXAMPLES OFF"
        "GLFW_BULID_DOCS OFF"
        "BUILD_SHARED_LIBS ON")

CPMAddPackage(NAME Freetype
    GITHUB_REPOSITORY freetype/freetype
    VERSION 2.12.1
    GIT_TAG e8ebfe988b5f57bfb9a3ecb13c70d9791bce9ecf
    OPTIONS
        "BUILD_SHARED_LIBS ON"
        "FT_DISABLE_HARFBUZZ OFF")
CPMAddPackage(NAME OpenAL
    GITHUB_REPOSITORY kcat/openal-soft
    VERSION 1.22.1
    GIT_TAG 05f5faf2655f4a51c69bfaacd4f67a740429f0dc
    OPTIONS
        "LIBTYPE SHARED"
        "ALSOFT_UTILS OFF"
        "ALSOFT_NO_CONFIG_UTIL ON"
        "ALSOFT_EXAMPLES OFF")

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/third_party/glad)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/third_party/stb_image)

if (${ARX_BUILD_TEST})
    CPMAddPackage(NAME GTest
        GITHUB_REPOSITORY google/googletest
        VERSION 1.12.0
        GIT_TAG f8d7d77c06936315286eb55f8de22cd23c188571
        OPTIONS
            "INSTALL_GTEST OFF"
            "gtest_force_shared_crt ON"
            "BUILD_SHARED_LIBS ON")
endif()

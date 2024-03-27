find_package(OpenGL REQUIRED)
set(CPM_SOURCE_CACHE "${CMAKE_CURRENT_LIST_DIR}/third_party/CPM")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchCPM.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM/CPM.cmake")
if (UNIX)
	set(CPM_LOCAL_PACKAGES_ONLY ON)
endif()
CPMFindPackage(NAME glm
    GITHUB_REPOSITORY g-truc/glm
    VERSION 0.9.9.8
    GIT_TAG 0.9.9.8) 
CPMFindPackage(NAME glfw3
    GITHUB_REPOSITORY glfw/glfw
    VERSION 3.3.1
    GIT_TAG ecda86fa4f89ecdd364e5a1a22645030fe0ced6e
    OPTIONS
        "GLFW_BUILD_TESTS OFF"
        "GLFW_BUILD_EXAMPLES OFF"
        "GLFW_BULID_DOCS OFF"
        "GLFW_INSTALL OFF"
        "BUILD_SHARED_LIBS OFF"
        "BUILD_STATIC_LIBS ON")

CPMFindPackage(NAME Freetype
    GITHUB_REPOSITORY freetype/freetype
    VERSION 2.12.1
    GIT_TAG e8ebfe988b5f57bfb9a3ecb13c70d9791bce9ecf
    OPTIONS
        "BUILD_SHARED_LIBS OFF"
        "BUILD_STATIC_LIBS ON"
        "FT_DISABLE_ZLIB ON"
        "FT_DISABLE_BZIP2 ON"
        "FT_DISABLE_PNG ON"
        "FT_DISABLE_HARFBUZZ ON"
        "FT_DISABLE_BROTLI ON"
        "FT_ENABLE_ERROR_STRINGS ON"
        "SKIP_INSTALL_ALL ON"
        "FT_CONFIG_OPTION_ERROR_STRINGS")
CPMFindPackage(NAME OpenAL
    GITHUB_REPOSITORY kcat/openal-soft
    VERSION 1.22.1
    GIT_TAG 05f5faf2655f4a51c69bfaacd4f67a740429f0dc
    OPTIONS
        "LIBTYPE STATIC"
        "ALSOFT_UTILS OFF"
        "ALSOFT_NO_CONFIG_UTIL ON"
        "ALSOFT_INSTALL OFF"
        "ALSOFT_INSTALL_CONFIG OFF" 
        "ALSOFT_INSTALL_HRTF_DATA OFF" 
        "ALSOFT_INSTALL_AMBDEC_PRESETS OFF"
        "ALSOFT_INSTALL_EXAMPLES OFF"
        "ALSOFT_INSTALL_UTILS OFF"
        "ALSOFT_EXAMPLES OFF")

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/third_party/glad")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/third_party/stb_image")

if (${ARX_BUILD_TEST})
    CPMFindPackage(NAME GTest
        GITHUB_REPOSITORY google/googletest
        VERSION 1.12.0
        GIT_TAG f8d7d77c06936315286eb55f8de22cd23c188571
        OPTIONS
            "INSTALL_GTEST OFF"
            "gtest_force_shared_crt ON"
            "BUILD_SHARED_LIBS OFF"
            "BUILD_STATIC_LIBS ON")
endif()

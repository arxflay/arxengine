find_package(Git REQUIRED)
execute_process(COMMAND git -C ${CMAKE_CURRENT_SOURCE_DIR} describe --candidates=0 --abbrev=0 --tags 
    OUTPUT_VARIABLE GIT_PROJ_TAG  ECHO_OUTPUT_VARIABLE ERROR_QUIET)

string(REGEX MATCH "v([0-9]+.)+[0-9]" VERSION_TAG "${GIT_PROJ_TAG}")

if (NOT VERSION_TAG)
    execute_process(COMMAND git -C ${CMAKE_CURRENT_SOURCE_DIR} rev-parse --short HEAD
        OUTPUT_VARIABLE COMMIT_ID ERROR_QUIET)
    string(STRIP ${COMMIT_ID} COMMIT_ID)
    execute_process(COMMAND git -C ${CMAKE_CURRENT_SOURCE_DIR} describe --abbrev=0 --tags 
        OUTPUT_VARIABLE GIT_PROJ_TAG ERROR_QUIET)
    string(REGEX MATCH "v([0-9]+.)+[0-9]" VERSION_TAG "${GIT_PROJ_TAG}")
endif()

if (VERSION_TAG)
    string(REGEX MATCH "[0-9]+" VER_MAJOR "${VERSION_TAG}")
    string(REGEX MATCH "\\.[0-9]+" VER_MINOR "${VERSION_TAG}")
    string(REGEX MATCH "[0-9]+" VER_MINOR "${VER_MINOR}")
    set(PKG_VER_NAME ${VERSION_TAG})
else()
    if (NOT COMMIT_ID)
        message(FATAL_ERROR "Project is not a git project")
    endif()
    message(WARNING "Project version tags not found")
    set(VER_MAJOR 0)
    set(VER_MINOR 0)
    set(PKG_VER_NAME "v${VER_MAJOR}.${VER_MINOR}")
endif()

if (COMMIT_ID)
    set(PKG_VER_NAME "${PKG_VER_NAME}(${COMMIT_ID})")
endif()

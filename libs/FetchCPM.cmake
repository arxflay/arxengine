set(CPM_VERSION "v0.38.2")

if(NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/cmake)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/cmake)
endif()
if (NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/cmake/cpmver.txt)
    file(TOUCH ${CMAKE_CURRENT_LIST_DIR}/cmake/cpmver.txt)
    file(WRITE ${CMAKE_CURRENT_LIST_DIR}/cmake/cpmver.txt " ")
endif()

file(READ ${CMAKE_CURRENT_LIST_DIR}/cmake/cpmver.txt CPM_CURRENT_VERSION)

if (NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/cmake/CPM.cmake OR (NOT (${CPM_CURRENT_VERSION} STREQUAL ${CPM_VERSION})))
    file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/download/${CPM_VERSION}/CPM.cmake ${CMAKE_CURRENT_LIST_DIR}/cmake/CPM.cmake TLS_VERIFY ON SHOW_PROGRESS)
    file(WRITE libs/cmake/cpmver.txt ${CPM_VERSION})
endif()

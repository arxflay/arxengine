 macro(arx_set_headers)
    set(oneValueArgs TARGET INCLUDE_DIR_PATH PRIVATE_FOLDER_NAME)
    cmake_parse_arguments(ARX_SET_HEADERS "" "${oneValueArgs}"
                          "" ${ARGN})
    
    if (NOT DEFINED ARX_SET_HEADERS_TARGET)
        message(ERROR "Target is not set or is empty")
    endif()

    if (NOT DEFINED ARX_SET_HEADERS_INCLUDE_DIR_PATH)
        message(ERROR "Include dir path is not set or is empty")
    endif()

    if (NOT DEFINED ARX_SET_HEADERS_PRIVATE_FOLDER_NAME)
        target_include_directories("${ARX_SET_HEADERS_TARGET}" PUBLIC "${ARX_SET_HEADERS_INCLUDE_DIR_PATH}")
        return()
    endif()

    set(ARX_SET_HEADERS_SCRIPT_NAME "arx_set_headers_${ARX_SET_HEADERS_TARGET}.cmake")
    set(HEADER_CACHE_FOLDER "${CMAKE_CURRENT_BINARY_DIR}/arx_header_cache")
    set(ARX_SET_HEADER_PATH "${HEADER_CACHE_FOLDER}/${ARX_SET_HEADERS_SCRIPT_NAME}")
    set(HEADER_PRIVATE_FOLDER_NAME "${ARX_SET_HEADERS_TARGET}_private_headers")
    set(HEADER_PRIVATE_FOLDER "${HEADER_CACHE_FOLDER}/${HEADER_PRIVATE_FOLDER_NAME}")
    set(HEADER_PUBLIC_FOLDER_NAME "${ARX_SET_HEADERS_TARGET}_public_headers")
    set(HEADER_PUBLIC_FOLDER "${HEADER_CACHE_FOLDER}/${HEADER_PUBLIC_FOLDER_NAME}")


    file(WRITE "${ARX_SET_HEADER_PATH}"
"    get_filename_component(FOLDER_NAME \"${ARX_SET_HEADERS_INCLUDE_DIR_PATH}\" NAME)
    file(REMOVE_RECURSE \"${HEADER_PRIVATE_FOLDER}\")
    file(REMOVE_RECURSE \"${HEADER_PUBLIC_FOLDER}\")
    file(COPY \"${ARX_SET_HEADERS_INCLUDE_DIR_PATH}\" DESTINATION \"${HEADER_CACHE_FOLDER}\")
    file(RENAME \"${HEADER_CACHE_FOLDER}/\${FOLDER_NAME}\" \"${HEADER_CACHE_FOLDER}/${HEADER_PUBLIC_FOLDER_NAME}\")
    file(GLOB_RECURSE INCLUDE_DIR_FILES LIST_DIRECTORIES TRUE \"${ARX_SET_HEADERS_INCLUDE_DIR_PATH}/*\")
    foreach (folder \${INCLUDE_DIR_FILES})
        if (IS_DIRECTORY \"\${folder}\")
            list(APPEND INCLUDE_DIR_DIRS \"\${folder}\")
        endif()
    endforeach()

    foreach (folder \${INCLUDE_DIR_DIRS})
        get_filename_component(FOLDER_NAME \"\${folder}\" NAME)
        if (\"\${FOLDER_NAME}\" STREQUAL \"${ARX_SET_HEADERS_PRIVATE_FOLDER_NAME}\")
            string(REPLACE \"${ARX_SET_HEADERS_INCLUDE_DIR_PATH}\" \"${HEADER_PUBLIC_FOLDER}\" PUBLIC_PATH_DEST \"\${folder}\")
            string(REPLACE \"${ARX_SET_HEADERS_INCLUDE_DIR_PATH}\" \"${HEADER_PRIVATE_FOLDER}\" PRIVATE_PATH_DEST \"\${folder}\")
            get_filename_component(PRIVATE_PATH_DEST_WITHOUT_NAME \"\${PRIVATE_PATH_DEST}\" DIRECTORY)
            file(REMOVE_RECURSE \"\${PUBLIC_PATH_DEST}\")
            file(COPY \"\${folder}\" DESTINATION \"\${PRIVATE_PATH_DEST_WITHOUT_NAME}\")
        endif()
    endforeach()")
    
    add_custom_target(copy_headers 
        COMMAND ${CMAKE_COMMAND} -P "${ARX_SET_HEADER_PATH}")
    add_dependencies("${ARX_SET_HEADERS_TARGET}" copy_headers)
    
    target_include_directories("${ARX_SET_HEADERS_TARGET}" PUBLIC "${HEADER_PUBLIC_FOLDER}")
    target_include_directories("${ARX_SET_HEADERS_TARGET}" PRIVATE "${HEADER_PRIVATE_FOLDER}")
    
endmacro()

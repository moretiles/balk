if(EXISTS "${BALK_DIRECTORIES_EXAMPLES}")
    file(
        GLOB_RECURSE
        balk_example_files
        CONFIGURE_DEPENDS
        "${BALK_DIRECTORIES_EXAMPLES}/*.cpp"
        "${BALK_DIRECTORIES_EXAMPLES}/*.cc"
        "${BALK_DIRECTORIES_EXAMPLES}/*.cxx"
        "${BALK_DIRECTORIES_EXAMPLES}/*.c++"
    )

    add_custom_target(examples)

    foreach(source_file ${balk_example_files})
        # Get the path relative to the examples source directory
        cmake_path(
            RELATIVE_PATH source_file
            BASE_DIRECTORY "${BALK_DIRECTORIES_EXAMPLES}"
            OUTPUT_VARIABLE rel_path
        )
    
        # Strip the .cpp extension
        string(REGEX REPLACE "\\.cpp$" "" target_rel_path "${rel_path}")
        string(REGEX REPLACE "\\.cc$" "" target_rel_path "${rel_path}")
        string(REGEX REPLACE "\\.cxx$" "" target_rel_path "${rel_path}")
        string(REGEX REPLACE "\\.c\\+\\+$" "" target_rel_path "${rel_path}")
    
        # Replace slashes with safe characters if needed
        string(REPLACE "/" "_" target_name "example_${target_rel_path}")
    
        add_executable(${target_name} EXCLUDE_FROM_ALL "${source_file}")

        # Preserve the directory structure in the binary output directory
        # e.g., ${BALK_DIRECTORIES_BUILD_EXAMPLES}/a/b/c/d.exe (or without .exe extension on Linux/Mac)
        get_filename_component(target_output_dir "${BALK_DIRECTORIES_BUILD_EXAMPLES}/${rel_path}" DIRECTORY)
        get_filename_component(target_output_name "${rel_path}" NAME_WE)
    
        set_target_properties(
            ${target_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${target_output_dir}"
            OUTPUT_NAME "${target_output_name}"
        )
    
        target_include_directories(${target_name} PRIVATE "${balk_header_files}")

        if(balk_modules_files)
            target_link_libraries(${target_name} PRIVATE balk_modules balk_FetchContent)
        else()
            target_link_libraries(${target_name} PRIVATE balk_FetchContent)
        endif()

        add_dependencies(examples ${target_name})
    endforeach()
endif()

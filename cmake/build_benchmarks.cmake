if(EXISTS "${BALK_DIRECTORY_BENCHMARKS}")
    set(BALK_DIRECTORY_BENCHMARK_BINARIES "${CMAKE_CURRENT_BINARY_DIR}/benchmarks")

    file(
        GLOB_RECURSE
        balk_benchmark_files
        CONFIGURE_DEPENDS
        "${BALK_DIRECTORY_BENCHMARKS}/*.cpp"
        "${BALK_DIRECTORY_BENCHMARKS}/*.cc"
    )

    add_custom_target(benchmarks)

    foreach(source_file ${balk_benchmark_files})
        # Get the path relative to the benchmarks source directory
        cmake_path(
            RELATIVE_PATH source_file
            BASE_DIRECTORY "${BALK_DIRECTORY_BENCHMARKS}"
            OUTPUT_VARIABLE rel_path
        )
    
        # Strip the .cpp extension
        string(REGEX REPLACE "\\.cpp$" "" target_rel_path "${rel_path}")
        string(REGEX REPLACE "\\.cc$" "" target_rel_path "${rel_path}")
    
        # Replace slashes with safe characters if needed
        string(REPLACE "/" "_" target_name "benchmark_${target_rel_path}")
    
        add_executable(${target_name} EXCLUDE_FROM_ALL "${source_file}")

        # Preserve the directory structure in the binary output directory
        # e.g., ${BALK_DIRECTORY_BENCHMARK_BINARIES}/a/b/c/d.exe (or without .exe extension on Linux/Mac)
        get_filename_component(target_output_dir "${BALK_DIRECTORY_BENCHMARK_BINARIES}/${rel_path}" DIRECTORY)
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

        add_dependencies(benchmarks ${target_name})
    endforeach()
endif()

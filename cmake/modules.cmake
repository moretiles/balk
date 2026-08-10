file(
    GLOB_RECURSE balk_modules_files CONFIGURE_DEPENDS
   "${BALK_DIRECTORY_SOURCE}/*.cppm"
)

if(balk_modules_files)
    # only true when balk_modules_files detects one or more files

    # make sure compiler supports modules
    include("${BALK_DIRECTORY_CMAKE}/module_test_capability.cmake")

    if(balk_modules_files)
        add_library(balk_modules)

        target_compile_features(balk_modules PUBLIC cxx_std_20)

        target_include_directories(balk_modules PRIVATE "${balk_header_files}")

        target_sources(
            balk_modules
            PUBLIC
            FILE_SET CXX_MODULES
            TYPE CXX_MODULES
            FILES ${balk_modules_files}
        )

        target_link_libraries(balk_modules PRIVATE balk_FetchContent)
    else()
        add_library(balk_modules INTERFACE)

        target_compile_features(balk_modules INTERFACE cxx_std_20)
    endif()
endif()

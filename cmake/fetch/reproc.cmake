# reproc
set(REPROC++ ON CACHE BOOL "Build reproc++" FORCE)
cmake_path(APPEND reproc_SOURCE_DIR "${FETCHCONTENT_BASE_DIR}" "reproc")
cmake_path(APPEND reproc_BINARY_DIR "${CMAKE_BINARY_DIR}" "reproc")
FetchContent_Declare(
    reproc
    GIT_REPOSITORY https://github.com/daandemeyer/reproc.git
    GIT_TAG v14.2.7
    SOURCE_DIR "${reproc_SOURCE_DIR}"
    BINARY_DIR "${reproc_BINARY_DIR}"
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(reproc)
target_link_libraries(balk_FetchContent INTERFACE reproc++)

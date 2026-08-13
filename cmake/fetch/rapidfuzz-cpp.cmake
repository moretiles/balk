# rapidfuzz-cpp
cmake_path(APPEND rapidfuzz_cpp_SOURCE_DIR "${FETCHCONTENT_BASE_DIR}" "rapidfuzz-cpp")
cmake_path(APPEND rapidfuzz_cpp_BINARY_DIR "${CMAKE_BINARY_DIR}" "rapidfuzz-cpp")
FetchContent_Declare(
    rapidfuzz_cpp
    GIT_REPOSITORY https://github.com/rapidfuzz/rapidfuzz-cpp.git
    GIT_TAG v3.3.3
    SOURCE_DIR "${rapidfuzz_cpp_SOURCE_DIR}"
    BINARY_DIR "${rapidfuzz_cpp_BINARY_DIR}"
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(rapidfuzz_cpp)
target_link_libraries(balk_FetchContent INTERFACE rapidfuzz::rapidfuzz)

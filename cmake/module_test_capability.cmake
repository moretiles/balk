include(CMakePushCheckState)

set(CMAKE_CXX_SCAN_FOR_MODULES ON)

set(CXX_MODULE_TEST_DIR "${CMAKE_BINARY_DIR}/module_test_capability")

file(WRITE "${CXX_MODULE_TEST_DIR}/CMakeLists.txt" [=[
cmake_minimum_required(VERSION 3.28)

project(cxx_module_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(test_module)
target_sources(test_module
    PUBLIC
        FILE_SET CXX_MODULES
        FILES
            test.cppm
)
]=])

file(WRITE "${CXX_MODULE_TEST_DIR}/test.cppm" [=[
export module test;

export int answer()
{
    return 42;
}
]=])

try_compile(
    HAS_CXX20_MODULES
    "${CMAKE_BINARY_DIR}/cxx_module_test_build"
    "${CXX_MODULE_TEST_DIR}"
    cxx_module_test
    OUTPUT_VARIABLE CXX_MODULE_TEST_OUTPUT
)

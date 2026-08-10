include(FetchContent)

set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/fetch")
add_library(balk_FetchContent INTERFACE)

file(
    GLOB_RECURSE CMAKE_FETCH_FILES CONFIGURE_DEPENDS
    "${BALK_DIRECTORY_CMAKE}/fetch/*.cmake"
)

foreach(CMAKE_FETCH_FILE ${CMAKE_FETCH_FILES})
    include("${CMAKE_FETCH_FILE}")
endforeach()

if("${BALK_BINARY}" STREQUAL "executable")
    add_executable(${PROJECT_NAME})
elseif("${BALK_BINARY}" STREQUAL "archive")
    add_library(${PROJECT_NAME} STATIC)
elseif("${BALK_BINARY}" STREQUAL "shared")
    add_library(${PROJECT_NAME} SHARED)
else()
    message(WARNING "Not building an executable, archive, or shared binary! Maybe an issue!")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE "${balk_header_files}")

file(
    GLOB_RECURSE
    balk_source_files
    CONFIGURE_DEPENDS
    "${BALK_DIRECTORY_SOURCE}/*.cpp"
    "${BALK_DIRECTORY_SOURCE}/*.cc"
)
target_sources(
    ${PROJECT_NAME}
    PRIVATE ${balk_source_files}
)

if(balk_modules_files)
    target_link_libraries(${PROJECT_NAME} PRIVATE balk_modules balk_FetchContent)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE balk_FetchContent)
endif()

if("${BALK_SETTINGS_BINARY}" STREQUAL "executable")
    add_executable(${PROJECT_NAME})
elseif("${BALK_SETTINGS_BINARY}" STREQUAL "archive")
    add_library(${PROJECT_NAME} STATIC)
elseif("${BALK_SETTINGS_BINARY}" STREQUAL "shared")
    add_library(${PROJECT_NAME} SHARED)
else()
    message(WARNING "Not building an executable, archive, or shared binary! Maybe an issue!")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE "${balk_header_files}")

file(
    GLOB_RECURSE
    balk_source_files
    CONFIGURE_DEPENDS
    "${BALK_DIRECTORIES_SOURCE}/*.cpp"
    "${BALK_DIRECTORIES_SOURCE}/*.cc"
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

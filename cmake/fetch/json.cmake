# json
cmake_path(APPEND json_SOURCE_DIR "${FETCHCONTENT_BASE_DIR}" "json")
cmake_path(APPEND json_BINARY_DIR "${CMAKE_BINARY_DIR}" "json")
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
    SOURCE_DIR "${json_SOURCE_DIR}"
    BINARY_DIR "${json_BINARY_DIR}"
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(json)
target_link_libraries(balk_FetchContent INTERFACE nlohmann_json::nlohmann_json)

# json schema
cmake_path(APPEND json_schema_validator_SOURCE_DIR "${FETCHCONTENT_BASE_DIR}" "json_schema_validator")
cmake_path(APPEND json_schema_validator_BINARY_DIR "${CMAKE_BINARY_DIR}" "json_schema_validator")
FetchContent_Declare(
    json_schema_validator
    GIT_REPOSITORY https://github.com/pboettch/json-schema-validator.git
    GIT_TAG 2.3.0
    SOURCE_DIR "${json_schema_validator_SOURCE_DIR}"
    BINARY_DIR "${json_schema_validator_BINARY_DIR}"
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(json_schema_validator)
target_link_libraries(balk_FetchContent INTERFACE nlohmann_json_schema_validator)

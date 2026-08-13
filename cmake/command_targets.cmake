add_custom_target(
    build
    DEPENDS ${PROJECT_NAME}
)

add_custom_target(
    run
    DEPENDS build
)

add_custom_target(
    export
    COMMAND ${CMAKE_COMMAND} -E make_directory "${BALK_DIRECTORIES_EXPORT}/${BALK_PROFILE}/${PROJECT_NAME}"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/${PROJECT_NAME}" "${BALK_DIRECTORIES_EXPORT}/${BALK_PROFILE}/${PROJECT_NAME}"
    DEPENDS ${PROJECT_NAME}
)

# CMake already defines a clean target
#add_custom_target(
    #clean
#)

add_custom_target(
    format
)

add_custom_target(
    lint
)

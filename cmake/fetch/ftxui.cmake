# ftxui
cmake_path(APPEND ftxui_SOURCE_DIR "${FETCHCONTENT_BASE_DIR}" "ftxui")
cmake_path(APPEND ftxui_BINARY_DIR "${CMAKE_BINARY_DIR}" "ftxui")
FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
    GIT_TAG v7.0.1
    SOURCE_DIR "${ftxui_SOURCE_DIR}"
    BINARY_DIR "${ftxui_BINARY_DIR}"
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(ftxui)
target_link_libraries(balk_FetchContent INTERFACE ftxui::screen INTERFACE ftxui::dom INTERFACE ftxui::component)

# control warning settings
if(${BALK_TOOLCHAIN} STREQUAL "msvc")
    add_compile_options(/W4)
elseif(${BALK_TOOLCHAIN} STREQUAL "gcc")
    add_compile_options(-Wall -Wextra -Wextra)
elseif(${BALK_TOOLCHAIN} STREQUAL "clang")
    add_compile_options(-Wall -Wextra -Wextra)
else()
endif()


set(warning_flags_library warning_flags)
add_library("${warning_flags_library}" INTERFACE)

if (MSVC)
    target_compile_options(
        "${warning_flags_library}"
        INTERFACE
        /W4 /WX
    )
else()
    target_compile_options(
        "${warning_flags_library}"
        INTERFACE
        -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Werror
    )
endif()

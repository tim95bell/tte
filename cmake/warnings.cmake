
set(warning_flags_library warning_flags)
add_library("${warning_flags_library}" INTERFACE)

if (MSVC)
    target_compile_options(
        "${warning_flags_library}"
        INTERFACE
        /W4
        /w14242
        /w14254
        /w14263
        /w14265
        /w14287
        /we4289
        /w14296
        /w14311
        /w14545
        /w14546
        /w14547
        /w14549
        /w14555
        /w14619
        /w14640
        /w14826
        /w14905
        /w14906
        /w14928
        /permissive-
    )
    if (TTE_WARNINGS_AS_ERRORS)
        target_compile_options(
            "${warning_flags_library}"
            INTERFACE
            /WX)
    endif()
else()
    target_compile_options(
        "${warning_flags_library}"
        INTERFACE
        -Wall
        -Wextra
        -Wextra-semi
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        -Wimplicit-fallthrough
        -Wno-format-nonliteral
    )
    if (NOT TTE_WARNING_LEVEL_STRICT)
        target_compile_options(
            "${warning_flags_library}"
            INTERFACE
            -Wno-parentheses
            -Wno-unused-function
            -Wno-unused-parameter
        )
    endif()
    if (TTE_WARNINGS_AS_ERRORS)
        target_compile_options(
            "${warning_flags_library}"
            INTERFACE
            -Werror)
    endif()
endif()

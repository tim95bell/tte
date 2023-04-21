
#pragma once

#include <tte/common/number_types.hpp>

namespace tte { namespace common {
    enum class KeyCode {
        Unknown,
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        Backspace,
        Space,
        Return,
    };


    struct Event {
        // TODO(TB): should WindowClose and WindowResized be a sub event of type Window?
        enum class Type { Unknown, Quit, KeyDown, KeyUp, WindowClose, WindowResized };

        struct KeyData {
            U8 repeat;
            KeyCode keycode;
        };

        Type type;

        union {
            KeyData key;
        };
    };
}}

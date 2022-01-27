#pragma once

#include <cstddef>

struct IOTypes {
    enum class Types {
        Undefined,
        Output_12V_Pwm,
        Output_12V,
        Output_12V_Low_Current,
        Output_Ground,
        Input_12V,
        Input_Ground,
        Input_5V,
        Input_5V_Variable,
        Input_Can,
        Count
    };

    static const char* Strings[(size_t)Types::Count];

    static const char* getString(Types type) { return Strings[(int)type]; }
};

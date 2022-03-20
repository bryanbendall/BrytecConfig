#pragma once

#include <string>

struct CanTypes {
    enum class Types {
        Disabled,
        Brytec,
        Custom,
        Count
    };

    static const char* Strings[(size_t)Types::Count];
    static const char* getString(Types type) { return Strings[(int)type]; }
};

struct CanFormat {
    enum class Types {
        Std,
        Ext,
        Count
    };

    static const char* Strings[(size_t)Types::Count];
    static const char* getString(Types type) { return Strings[(int)type]; }
};

struct CanSpeed {
    enum class Types {
        Speed_1MBps,
        Speed_500kBps,
        Speed_250kBps,
        Speed_200kBps,
        Speed_125kBps,
        Speed_100kBps,
        Speed_50kBps,
        Speed_20kBps,
        Count
    };

    static const char* Strings[(size_t)Types::Count];
    static const char* getString(Types type) { return Strings[(int)type]; }
};

struct CanBus {
    std::string name;
    CanTypes::Types type = CanTypes::Types::Disabled;
    CanFormat::Types format = CanFormat::Types::Std;
    CanSpeed::Types speed = CanSpeed::Types::Speed_1MBps;
};
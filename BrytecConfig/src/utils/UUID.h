#pragma once

#include <functional>

namespace Brytec {

class UUID {
public:
    UUID();
    UUID(uint64_t uuid);
    UUID(const UUID& other);

    operator uint64_t() { return m_UUID; }
    operator const uint64_t() const { return m_UUID; }

private:
    uint64_t m_UUID;
};

}

namespace std {

template <>
struct hash<Brytec::UUID> {
    std::size_t operator()(const Brytec::UUID& uuid) const
    {
        return (uint64_t)uuid;
    }
};
}
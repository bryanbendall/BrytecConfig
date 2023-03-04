#pragma once

#include <filesystem>
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>

class BinarySerializer {

public:
    template <typename T>
    void writeRaw(T data)
    {
        int size = sizeof(T);
        uint8_t* bitData = (uint8_t*)(&data);
        for (int i = 0; i < sizeof(T); i++) {
            m_data.push_back(bitData[i]);
        }
    }

    template <typename T>
    void writeArray(const T* data, uint32_t size)
    {
        writeRaw<uint32_t>(size);
        for (uint32_t i = 0; i < size; i++)
            writeRaw<T>(data[i]);
    }

    void append(BinarySerializer& ser);
    void writeToFile(const std::filesystem::path& path);

    std::vector<uint8_t>& getData() { return m_data; }

private:
    std::vector<uint8_t> m_data;
};

template <>
inline void BinarySerializer::writeRaw<std::string>(std::string data)
{
    writeArray<char>(data.data(), data.size());
}

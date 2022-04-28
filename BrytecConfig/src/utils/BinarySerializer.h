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
        for (int i = 0; i < size; i++)
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

class BinaryDeserializer {

public:
    BinaryDeserializer(uint8_t* data)
        : m_data(data)
    {
    }

    BinaryDeserializer(std::filesystem::path path);

    ~BinaryDeserializer();

    template <typename T>
    T readRaw()
    {
        T temp = 0;
        for (int i = m_currentOffset; i < m_currentOffset + sizeof(T); i++) {
            temp |= (T)m_data[i] << ((i - m_currentOffset) * 8);
        }
        m_currentOffset += sizeof(T);
        return temp;
    }

private:
    bool m_ownData = false;
    uint8_t* m_data;
    uint64_t m_currentOffset = 0;
};

template <>
inline std::string BinaryDeserializer::readRaw<std::string>()
{
    uint32_t length = readRaw<uint32_t>();
    char data[length];

    for (int i = 0; i < length; i++)
        data[i] = readRaw<char>();

    return std::string(data, length);
}
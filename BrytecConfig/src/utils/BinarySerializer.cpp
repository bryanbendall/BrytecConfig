#include "BinarySerializer.h"
#include <fstream>

void BinarySerializer::append(BinarySerializer& ser)
{
    m_data.reserve(m_data.size() + ser.getData().size());
    m_data.insert(m_data.end(), ser.getData().begin(), ser.getData().end());
}

void BinarySerializer::writeToFile(const std::filesystem::path& path)
{
    std::ofstream fout(path, std::ofstream::binary);
    fout.write((char*)m_data.data(), m_data.size());
}

BinaryDeserializer::BinaryDeserializer(std::filesystem::path path)
{
    std::ifstream is(path, std::ifstream::binary);
    if (is) {
        is.seekg(0, is.end);
        int length = is.tellg();
        is.seekg(0, is.beg);

        m_data = new uint8_t[length];
        is.read((char*)m_data, length);

        is.close();

        m_ownData = true;
    }
}

BinaryDeserializer ::~BinaryDeserializer()
{
    if (m_ownData)
        delete[] m_data;
}
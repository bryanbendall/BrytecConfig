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

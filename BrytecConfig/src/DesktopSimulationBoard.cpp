#include "Boards/BrytecBoard.h"

#include "Deserializer/BinaryArrayDeserializer.h"
#include <iostream>
#include <vector>

namespace Brytec {

std::vector<uint8_t> config;
BinaryArrayDeserializer deserializer(nullptr, 0);

BinaryDeserializer* BrytecBoard::getDeserializer()
{
    BinaryArrayDeserializer des(config.data(), config.size());
    deserializer = des;
    return &deserializer;
}

void BrytecBoard::error(EBrytecErrors error)
{
    switch (error) {
    case EBrytecErrors::ModuleHeader:
        std::cout << "Module header is wrong" << std::endl;
        break;
    case EBrytecErrors::NodeGroupHeader:
        std::cout << "Node Group header is wrong" << std::endl;
        break;
    case EBrytecErrors::ModuleNotEnabled:
        std::cout << "This module is not enabled" << std::endl;
        break;
    case EBrytecErrors::AddNodeFailed:
        std::cout << "Failed to add node" << std::endl;
        break;
    case EBrytecErrors::NodeVectorOutOfSpace:
        std::cout << "Node Vector out of space" << std::endl;
        break;
    case EBrytecErrors::FailedToCreateNode:
        std::cout << "Failed to create node" << std::endl;
        break;
    case EBrytecErrors::NodeIndexOutOfBounds:
        std::cout << "Node index out of bounds" << std::endl;
        break;
    case EBrytecErrors::BadAlloc:
        std::cout << "Node group allocation failed" << std::endl;
        break;
    case EBrytecErrors::CanBufferFull:
        std::cout << "Can Buffer Full" << std::endl;
        break;

    default:
        std::cout << "Unknown Error" << std::endl;
        break;
    }
}

void BrytecBoard::setupCan(uint8_t index, CanSpeed::Types speed)
{
}

void BrytecBoard::setupPin(uint16_t index, IOTypes::Types type)
{
}

void BrytecBoard::shutdownAllPins()
{
}

float BrytecBoard::getPinValue(uint16_t index, IOTypes::Types type)
{
    return 0.0f;
}

float BrytecBoard::getPinVoltage(uint16_t index)
{
    return 0.0f;
}

float BrytecBoard::getPinCurrent(uint16_t index)
{
    return 0.0f;
}

void BrytecBoard::setPinValue(uint16_t index, IOTypes::Types type, float value)
{
}

void BrytecBoard::sendCan(uint8_t index, const CanFrame& frame)
{
}

void BrytecBoard::sendBrytecCanUsb(const CanFrame& frame)
{
}

void BrytecBoard::ReserveConfigSize(uint16_t size)
{
    if (config.size() < size)
        config.reserve(size);
}

void BrytecBoard::updateConfig(uint8_t* data, uint32_t size, uint32_t offset)
{
    ReserveConfigSize(size + offset);
    config.insert(config.begin() + offset, data, data + size);
}

uint32_t BrytecBoard::getTemplateSize()
{
    return 0;
}

void BrytecBoard::getTemplateData(uint8_t* dest, uint32_t offset, uint32_t length)
{
}

uint32_t BrytecBoard::getConfigSize()
{
    return 0;
}

void BrytecBoard::getConfigData(uint8_t* dest, uint32_t offset, uint32_t length)
{
}

void BrytecBoard::AddedNamesNodeGroup(uint16_t index, std::string ngName)
{
}
}
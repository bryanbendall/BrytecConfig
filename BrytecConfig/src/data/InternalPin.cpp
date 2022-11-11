#include "InternalPin.h"

InternalPin::InternalPin(const std::vector<IOTypes::Types>& availableTypes)
{
    setAvailableTypes(availableTypes);
}

InternalPin::~InternalPin()
{
}
#include "InternalPin.h"

namespace Brytec {

InternalPin::InternalPin(const std::vector<IOTypes::Types>& availableTypes)
{
    setAvailableTypes(availableTypes);
}

InternalPin::~InternalPin()
{
}

}
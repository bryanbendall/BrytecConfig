#pragma once

#include "Pin.h"

class InternalPin : public Pin {

public:
    InternalPin(const std::vector<IOTypes::Types>& availableTypes);
    virtual ~InternalPin();
};
#pragma once

#include <memory>
#include "ITransport.hpp"

class TransportBuilder
{
public:
    static std::unique_ptr<ITransport> createUart();
};
#include "TransportBuilder.hpp"

#include "UartBroker.hpp"

std::unique_ptr<ITransport> TransportBuilder::createUart()
{
    return std::make_unique<UartBroker>();
}
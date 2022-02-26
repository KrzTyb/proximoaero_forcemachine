#pragma once

#include <string>
#include <memory>

class TransportDelegate;

class ITransport
{
public:

    virtual ~ITransport() {}

    virtual void send(const std::string& message) = 0;

    virtual void setReceiver(TransportDelegate* receiver) = 0;

};

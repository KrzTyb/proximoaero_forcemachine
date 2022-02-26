#pragma once

#include <string>

class TransportDelegate
{
public:
    virtual ~TransportDelegate() {}
    virtual void onMessageReceived(const std::string& message) = 0;

};

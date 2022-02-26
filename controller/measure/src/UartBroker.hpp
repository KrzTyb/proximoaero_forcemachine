#pragma once

#include "ITransport.hpp"

#include <QSerialPort>
#include <QObject>

class UartBroker : public ITransport, public QObject
{
public:
    UartBroker();
    ~UartBroker();

    void send(const std::string& message) override;

    void setReceiver(TransportDelegate* receiver) override;

private slots:
    void onReadyRead();

private:

    QSerialPort m_serialPort;

    TransportDelegate* m_receiver;

    QByteArray m_buffer;

    std::string m_messageToSend;
};

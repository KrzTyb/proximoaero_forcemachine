#include "UartBroker.hpp"

#include <QSerialPortInfo>

#include "TransportDelegate.hpp"

#include <QDebug>

constexpr auto UartPort = "ttyS0";

constexpr auto UartBaudRate = 115200;
// constexpr auto UartBaudRate = 921600;

UartBroker::UartBroker()
    : QObject(nullptr)
{
    auto port = QSerialPortInfo{"ttyS0"};
    m_serialPort.setPort(port);

    m_serialPort.setBaudRate(UartBaudRate);

    m_serialPort.open(QIODeviceBase::ReadWrite);
    m_serialPort.clear();

    connect(&m_serialPort, &QIODevice::readyRead, this, &UartBroker::onReadyRead);
}

UartBroker::~UartBroker()
{
    m_serialPort.flush();
    m_serialPort.close();
}

void UartBroker::send(const std::string& message)
{
    m_messageToSend = message;

    m_messageToSend += "\n";
    // qDebug() << "Message sended: [" << QString::fromStdString(m_messageToSend) << "]";
    m_serialPort.write(m_messageToSend.c_str(), m_messageToSend.size());
}

void UartBroker::setReceiver(TransportDelegate* receiver)
{
    m_receiver = receiver;
}

void UartBroker::onReadyRead()
{
    while (m_serialPort.bytesAvailable())
    {
        auto character = m_serialPort.read(1);

        if (character.toStdString() != "\n")
        {
            m_buffer += character;
        }
        else
        {
            auto strBuffer = m_buffer.toStdString();
            // qDebug() << "Message received: [" << QString::fromStdString(strBuffer) << "]";
            m_receiver->onMessageReceived(strBuffer);
            m_buffer.clear();
        }
    }
}

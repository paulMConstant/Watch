#include "client.h"

#include <QByteArray>
#include <QDataStream>
#include <QHostAddress>
#include <QTcpSocket>

#include <Messages/constants.h>
#include "Logger/logger.h"

Client::Client() noexcept
{
    connect(socket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
}

Client::~Client() noexcept
{
    socket->disconnectFromHost();
    socket->deleteLater();
}

bool Client::isConnectedToServer() const noexcept
{
    return (socket->state() == QTcpSocket::ConnectedState);
}

void Client::connectToServer(const QString& IP) noexcept
{
    if (isConnectedToServer())
    {
        disconnectFromServer();
    }

    socket->connectToHost(IP, Constants::port);
}

void Client::disconnectFromServer() noexcept
{
    socket->disconnectFromHost();
}

void Client::sendChat(QString chatMessage) noexcept
{
    auto htmlName = name;
    htmlName.replace('<', "&lt;");
    htmlName.replace('>', "&gt;");
    chatMessage = "<b>" + htmlName + " :</b> " + chatMessage;
    Logger::printChatMsg(chatMessage);
    sendMessage(Message(Message::Type::Chat, chatMessage));
}

void Client::sendTimestamp(const Timestamp& timestamp) noexcept
{
    sendMessage(Message(Message::Type::Timestamp, QVariant::fromValue<Timestamp>(timestamp)));
}

void Client::setName(const QString& name) noexcept
{
    if (name == this->name)
    {
        return;
    }
    this->name = name;
    sendMessage(Message(Message::Type::Name, name));
}

void Client::onConnected() noexcept
{
    sendMessage(Message(Message::Type::Name, name));
    Logger::printInfo("Connection succesful");
    emit connected();
}

void Client::sendMessage(const Message& message) noexcept
{
    if (socket->isOpen())
    {
        socket->write(message.toByteArray());
        socket->flush();
    }
}

void Client::onDisconnected() noexcept
{
    Logger::printInfo("Disconnected.");
    emit disconnected();
}

void Client::socketError() noexcept
{
    Logger::printError("Socket error : " + socket->errorString());
}

void Client::dataReceived() noexcept
{
    while (socket->bytesAvailable())
    {
        QDataStream in(socket);
        if (msgSize == 0)
        {
            if (socket->bytesAvailable() < static_cast<int>(sizeof(quint16)))
            {
                // Did not receive entire message size
                return;
            }
            in >> msgSize;
        }
        if (socket->bytesAvailable() < msgSize)
        {
            return;
        }
        auto msg = Message();
        in >> msg;
        processMessage(msg);

        msgSize = 0;
    }
}

void Client::processMessage(const Message& message) noexcept
{
    switch(message.type)
    {
    case Message::Type::Name:
        emit connectionsChanged(message.data.toStringList());
        break;

    case Message::Type::Timestamp:
        emit timestampChanged(message.data.value<Timestamp>());
        break;

    case Message::Type::Chat:
        Logger::printChatMsg(message.data.toString());
        break;

    default:
        Logger::printError("Received invalid message...");
    }
}

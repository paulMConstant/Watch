#include "server.h"

#include <QTcpSocket>
#include <QDataStream>

#include <algorithm>

#include <Messages/constants.h>
#include "Logger/logger.h"

Server::Server() noexcept
{
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    if (server->listen(QHostAddress::Any, Constants::port) == false)
    {
        Logger::print("Could not initialize server : " + server->errorString() + '.');
        exit(1);
    }
    else
    {
        Logger::print("Server listening on port " + QString::number(Constants::port) + '.');
    }
}

Server::~Server() noexcept
{
    server->close();
}

void Server::newConnection() noexcept
{
    auto socket = server->nextPendingConnection();
    connect(socket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError()));
}

void Server::dataReceived() noexcept
{
    auto socket = qobject_cast<QTcpSocket*>(sender());
    if (socket == nullptr)
    {
        return;
    }
    while(socket->bytesAvailable())
    {
        QDataStream in(socket);
        if (connectedClients[socket].msgSize == 0)
        {
            if (socket->bytesAvailable() < static_cast<int>(sizeof(quint16)))
            {
                // Did not receive entire message size
                return;
            }
            in >> connectedClients[socket].msgSize;
        }
        if (socket->bytesAvailable() < connectedClients[socket].msgSize)
        {
            return;
        }
        auto msg = Message();
        in >> msg;
        processMessage(msg, socket);

        connectedClients[socket].msgSize = 0;
    }
}

void Server::processMessage(const Message &message, QTcpSocket* source) noexcept
{
    switch(message.type)
    {
    case Message::Type::Name:
        registerClientName(message.data.toString(), source);
        break;

    case Message::Type::Timestamp:
    case Message::Type::Chat:
        broadcast(message, source);
        break;

    default:
        Logger::print("Received invalid message...");
    }
}

void Server::registerClientName(const QString &name, QTcpSocket *source) noexcept
{
    Logger::print("New client or name change : " + name);
    connectedClients[source] = Client(name);
    sendConnectedClientsList();
}

void Server::sendConnectedClientsList() noexcept
{
    auto clientNames = QStringList();
    for (const auto& client : connectedClients.values())
    {
        clientNames << client.name;
    }
    broadcast(Message(Message::Type::Name, clientNames));
}

#include <Messages/timestamp.h>
void Server::broadcast(const Message& message, QTcpSocket* source) noexcept
{
    for (auto socket : connectedClients.keys())
    {
        if (socket != source)
        {
            socket->write(message.toByteArray());
            socket->flush();
        }
    }
}

void Server::clientDisconnected() noexcept
{
    auto socket = qobject_cast<QTcpSocket*>(sender());
    if (socket == nullptr)
    {
        return;
    }

    Logger::print(connectedClients[socket].name + " disconnected.");
    connectedClients.remove(socket);
    sendConnectedClientsList();

}

void Server::socketError() noexcept
{
    auto socket = qobject_cast<QTcpSocket*>(sender());
    if (socket == nullptr)
    {
        return;
    }
    Logger::print("Error : " + socket->errorString());
}

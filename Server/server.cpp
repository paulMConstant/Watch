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
    auto client = server->nextPendingConnection();
    connect(client, SIGNAL(readyRead()), this, SLOT(dataReceived()));
    connect(client, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    connect(client, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError()));
}

void Server::dataReceived() noexcept
{
    auto client = qobject_cast<QTcpSocket*>(sender());
    if (client == nullptr)
    {
        return;
    }

    QDataStream in(client);
    if (msgSize == 0)
    {
        if (client->bytesAvailable() < static_cast<int>(sizeof(quint16)))
        {
            // Did not receive entire message size
            return;
        }
        in >> msgSize;
    }
    if (client->bytesAvailable() < msgSize)
    {
        return;
    }
    auto msg = Message();
    in >> msg;
    processMessage(msg, client);

    msgSize = 0;
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
    connectedClients.remove(Client(source));

    connectedClients << Client(source, name);
    sendConnectedClientsList();
}

void Server::sendConnectedClientsList() noexcept
{
    auto clientNames = QStringList();
    std::transform(connectedClients.cbegin(), connectedClients.cend(),
                  std::back_inserter(clientNames),
                  [](const auto& client) { return client.name; });
    broadcast(Message(Message::Type::Name, clientNames));
}

void Server::broadcast(const Message& message, QTcpSocket* source) noexcept
{
    for (auto& client : connectedClients)
    {
        if (client.socket != source)
        {
            client.socket->write(message.toByteArray());
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
    auto client = std::find_if(connectedClients.cbegin(), connectedClients.cend(),
                               [socket](const auto& client)
    { return client.socket == socket; });
    if (client == connectedClients.cend())
    {
        return;
    }
    Logger::print(client->name + " disconnected.");
    connectedClients.remove(*client);
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

#include "Server/server.h"

#include <Messages/constants.h>

#include <QDataStream>
#include <QHostAddress>
#include <QSslKey>
#include <QFile>

#include <algorithm>

#include "Logger/logger.h"

Server::Server() noexcept
{
    password.readFromUser();

    if (listen(QHostAddress::Any, Constants::port) == false)
    {
        Logger::print("Could not initialize server : " + errorString() + '.');
        exit(1);
    }
    Logger::print("Server listening on port " + QString::number(Constants::port) + '.');
}

Server::~Server() noexcept
{
    close();
}

void Server::incomingConnection(qintptr handle)
{
    Logger::print("Received incoming connection.");
    auto socket = new QSslSocket(this);
    socket->setSocketDescriptor(handle);
    socket->setSslConfiguration(sslConfig());
    socket->startServerEncryption();

    connect(socket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError()));
    connect(socket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslError(QList<QSslError>)));
}

void Server::dataReceived() noexcept
{
    auto socket = qobject_cast<QSslSocket*>(sender());
    if (socket == nullptr)
    {
        return;
    }
    while (socket->bytesAvailable())
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

void Server::processMessage(const Message& message, QSslSocket* source) noexcept
{
    if (connectedClients[source].authenticated == false)
    {
        if (message.type == Message::Type::Hello)
        {
            registerClient(message.data.value<Hello>(), source);
        }
        else
        {
            Logger::print("First message from client is not hello. Disconnecting.");
            source->disconnectFromHost();
        }
    }
    else
    {
        broadcast(message, source);
    }
}

void Server::registerClient(const Hello& hello, QSslSocket* source) noexcept
{
    Logger::print("New connection : " + hello.name);
    if (password == hello.password)
    {
        Logger::print("Authentication succesful : " + hello.name);
        connectedClients[source] = Client(hello.name, true);
        send(Message(Message::Type::Hello, 0), source);  // ACK
        sendConnectedClientsList();
    }
    else
    {
        Logger::print("Rejected client '" + hello.name + "' connecting with wrong password.");
        connectedClients[source] = Client(hello.name, false);
        source->disconnectFromHost();
    }
}

void Server::sendConnectedClientsList() noexcept
{
    auto clientNames = QStringList();
    for (const auto& client : connectedClients.values())
    {
        clientNames << client.name;
    }
    broadcast(Message(Message::Type::Names, clientNames));
}

void Server::broadcast(const Message& message, QSslSocket* source) noexcept
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

void Server::send(const Message& message, QSslSocket* destination) noexcept
{
    destination->write(message.toByteArray());
    destination->flush();
}

void Server::clientDisconnected() noexcept
{
    auto socket = qobject_cast<QSslSocket*>(sender());
    if (socket == nullptr)
    {
        return;
    }
    auto userAuthenticated = connectedClients[socket].authenticated;
    Logger::print(connectedClients[socket].name + " disconnected.");
    connectedClients.remove(socket);
    socket->deleteLater();
    if (userAuthenticated)
    {
        sendConnectedClientsList();
    }
}

void Server::socketError() noexcept
{
    auto socket = qobject_cast<QSslSocket*>(sender());
    if (socket == nullptr)
    {
        return;
    }
    Logger::print("Error : " + socket->errorString());
}

void Server::sslError(QList<QSslError> errors) noexcept
{
    auto socket = qobject_cast<QSslSocket*>(sender());
    if (socket == nullptr)
    {
        return;
    }
    socket->disconnectFromHost();
    if (errors.empty())
    {
        return;
    }
    Logger::print("Error : " + errors.takeAt(0).errorString());
}

QSslConfiguration Server::sslConfig() const noexcept
{
    auto config = QSslConfiguration();
    auto certificate = QSslCertificate::fromPath(":/certs/cert");
    Q_ASSERT_X(certificate.size(), "Server::sslConfig()",
               "No certificate found in qrc. Run /certs/create_server_certificate.sh");
    config.setLocalCertificate(certificate.at(0));
    QFile key(":/certs/pkey");
    key.open(QIODevice::ReadOnly);
    config.setPrivateKey(QSslKey(key.readAll(), QSsl::Rsa));
    key.close();
    config.setCaCertificates(QSslCertificate::fromPath(":/certs/client_cert"));
    config.setPeerVerifyMode(QSslSocket::VerifyPeer);
    return config;
}

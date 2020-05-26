#include "client.h"

#include <QByteArray>
#include <QDataStream>
#include <QHostAddress>
#include <QSslSocket>
#include <QSslKey>
#include <QSslCertificate>

#include <Messages/constants.h>
#include "Logger/logger.h"

Client::Client() noexcept
{
    socket->addCaCertificates(":/server_cert");
    connect(socket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError()));
    connect(socket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslError(QList<QSslError>)));
    connect(socket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
}

Client::~Client() noexcept
{
    socket->disconnectFromHost();
    socket->deleteLater();
}

bool Client::isConnectedToServer() const noexcept
{
    return (socket->state() == QSslSocket::ConnectedState);
}

void Client::connectToServer(const QString& IP) noexcept
{
    if (isConnectedToServer())
    {
        disconnectFromServer();
    }
    socket->connectToHostEncrypted(IP, Constants::port);
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

void Client::sendURL(const QString& URL) noexcept
{
    if (isConnectedToServer())
    {
        sendMessage(Message(Message::Type::URL, URL));
        Logger::printInfo("Shared current media.");
    }
    else
    {
        Logger::printError("Cannot share current media : you are not connected to the server.");
    }
}

void Client::sendName() noexcept
{
    sendMessage(Message(Message::Type::Name, name));
}

void Client::setName(const QString& name) noexcept
{
    if (name == this->name)
    {
        return;
    }
    this->name = name;
    sendName();
}

void Client::onConnected() noexcept
{
    sendName();
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

void Client::sslError(QList<QSslError> errors) noexcept
{
    disconnectFromServer();
    if (errors.empty())
    {
        return;
    }
    auto error = errors.takeFirst();
    Logger::printError("Ssl error : " + error.errorString());
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

    case Message::Type::URL:
        Logger::printChatMsg("J'ai reçu un URL");
        emit urlChanged(message.data.toString());
        break;

    default:
        Logger::printError("Received invalid message...");
    }
}

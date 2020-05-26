#include "Network/client.h"

#include <QCoreApplication>
#include <QByteArray>
#include <QDataStream>
#include <QHostAddress>
#include <QSslSocket>
#include <QSslKey>
#include <QSslCertificate>
#include <QSound>
#include <QMediaPlayer>

#include <Messages/constants.h>
#include "Logger/logger.h"
#include "Helpers/helpers.h"

Client::Client() noexcept
{
    notificationPlayer.setVolume(50);
    socket->addCaCertificates(":/certs/server_cert");
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

void Client::sendTimestamp(const Timestamp& timestamp) noexcept
{
    sendMessage(Message(Message::Type::Timestamp, QVariant::fromValue<Timestamp>(timestamp)));
}

void Client::sendChat(QString chatMessage) noexcept
{
    chatMessage = namedMessage(chatMessage);
    Logger::printBlack(chatMessage);
    sendMessage(Message(Message::Type::Chat, chatMessage));
}

void Client::sendInfo(QString info) noexcept
{
    info = "<i>" + info + "</i>";
    info = namedMessage(info);
    sendMessage(Message(Message::Type::Info, info));
}

void Client::sendMedia(const QString& media) noexcept
{
    if (isConnectedToServer())
    {
        sendMessage(Message(Message::Type::Media, media));
        Logger::printGreen("Shared current media.");
    }
    else
    {
        Logger::printRed("Cannot share current media : you are not connected to the server.");
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
    Logger::printGreen("Connection succesful");
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
    Logger::printGreen("Disconnected.");
    emit disconnected();
}

void Client::socketError() noexcept
{
    Logger::printRed("Socket error : " + socket->errorString());
}

void Client::sslError(QList<QSslError> errors) noexcept
{
    disconnectFromServer();
    if (errors.empty())
    {
        return;
    }
    auto error = errors.takeFirst();
    Logger::printRed("Ssl error : " + error.errorString());
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
    switch (message.type)
    {
        case Message::Type::Name:
            emit connectionsChanged(message.data.toStringList());
            break;

        case Message::Type::Timestamp:
            emit timestampChanged(message.data.value<Timestamp>());
            break;

        case Message::Type::Chat:
            notificationPlayer.setMedia(QUrl("qrc:/sounds/chat"));
            notificationPlayer.play();
            [[fallthrough]];
        case Message::Type::Info:
            Logger::printBlack(message.data.toString());
            break;

        case Message::Type::Media:
            emit mediaChanged(message.data.toString());
            break;

        default:
            Logger::printRed("Received invalid message...");
    }
}

QString Client::namedMessage(QString msg) noexcept
{
    auto htmlName = Helpers::String::cleanHtml(name);
    msg = "<b>" + htmlName + " :</b> " + msg;
    return msg;
}

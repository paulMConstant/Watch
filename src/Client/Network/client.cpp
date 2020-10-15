#include "Network/client.h"

#include <QCoreApplication>
#include <QByteArray>
#include <QDataStream>
#include <QSslKey>
#include <QSslCertificate>
#include <QInputDialog>
#include <QFile>

#include <Messages/constants.h>
#include <Messages/hello.h>
#include <Messages/passwordconventions.h>
#include "Logger/logger.h"
#include "Helpers/helpers.h"

Client::Client()
{
    socket->setSslConfiguration(sslConfig());

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

void Client::setName(const QString& name) noexcept
{
    if (name == this->name)
    {
        return;
    }
    this->name = name;
}

void Client::onConnected() noexcept
{
    bool ok;
    auto password = QInputDialog::getText(NULL,
                                          tr("Password ?"),
                                          tr("Enter the server password :"),
                                          QLineEdit::Password,
                                          "", &ok);
    if (ok)
    {
        auto hash = PasswordConventions::hash(password);
        sendMessage(Message(Message::Type::Hello, QVariant::fromValue<Hello>(Hello(hash, name))));
        waitingForPasswordACK = true;
    }
    else
    {
        disconnectFromServer();
    }
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
    if (waitingForPasswordACK)
    {
        Logger::printRed("Invalid password.");
    }
    else
    {
        Logger::printGreen("Disconnected.");
    }
    waitingForPasswordACK = false;
    emit disconnected();
}

void Client::socketError() noexcept
{
    Logger::printRed("Socket error : " + socket->errorString());
}

void Client::sslError(QList<QSslError> errors) noexcept
{
    auto error = errors.takeFirst();
    if (error.error() == QSslError::HostNameMismatch)
    {
        // Error is safe to ignore. Server identity is not verified.
        return;
    }

    disconnectFromServer();
    if (errors.empty())
    {
        return;
    }
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
        case Message::Type::Hello:
            Logger::printGreen("Connection successful");
            waitingForPasswordACK = false;
            emit connected();
            break;

        case Message::Type::Names:
            emit connectionsChanged(message.data.toStringList());
            break;

        case Message::Type::Timestamp:
            emit timestampChanged(message.data.value<Timestamp>());
            break;

        case Message::Type::Chat:
            Logger::playChatSound();
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

QSslConfiguration Client::sslConfig() const noexcept
{
    auto config = QSslConfiguration();
    auto certificate = QSslCertificate::fromPath(":/certs/cert");
    Q_ASSERT_X(certificate.size(), "Client::sslConfig()", "No certificate found in qrc. "
               "Run /certs/create_client_certificate.sh");
    config.setLocalCertificate(certificate.at(0));
    QFile key(":/certs/pkey");
    key.open(QIODevice::ReadOnly);
    config.setPrivateKey(QSslKey(key.readAll(), QSsl::Rsa));
    key.close();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    return config;
}

QString Client::namedMessage(QString msg) const noexcept
{
    auto htmlName = Helpers::String::cleanHtml(name);
    msg = "<b>" + htmlName + " :</b> " + msg;
    return msg;
}

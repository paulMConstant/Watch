#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QString>
#include <QTcpServer>
#include <QSslSocket>
#include <QSslError>
#include <QSslConfiguration>
#include <QList>

#include <Messages/message.h>
#include <Messages/hello.h>

#include "password.h"  // NOLINT[build/include_subdir]

struct Client
{
    explicit Client(const QString& name = "Unknown", bool authenticated = false) :
        name(name), authenticated(authenticated)
    {
    }

    QString name;
    bool authenticated;
    quint16 msgSize = 0;
};


class Server : public QTcpServer
{
    Q_OBJECT
  public:
    Server() noexcept;
    ~Server() noexcept;

  private slots:
    void dataReceived() noexcept;
    void clientDisconnected() noexcept;
    void socketError() noexcept;
    void sslError(QList<QSslError> errors) noexcept;

  protected:
    void incomingConnection(qintptr handle) override;

  private:
    QMap<QSslSocket*, Client> connectedClients;
    Password password;

    void processMessage(const Message& message, QSslSocket* source) noexcept;
    void registerClient(const Hello& hello, QSslSocket* source) noexcept;
    void sendConnectedClientsList() noexcept;
    void broadcast(const Message& message, QSslSocket* source = nullptr) noexcept;
    void send(const Message& message, QSslSocket* destination) noexcept;
    QSslConfiguration sslConfig() const noexcept;
};

#endif  // SERVER_H

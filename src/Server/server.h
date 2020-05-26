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

struct Client
{
    explicit Client(const QString& name = "Unknown") : name(name)
    {
    }

    QString name = "Unknown";
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

    void processMessage(const Message& message, QSslSocket* source) noexcept;
    void registerClientName(const QString& name, QSslSocket* source) noexcept;
    void sendConnectedClientsList() noexcept;
    void broadcast(const Message& message, QSslSocket* source = nullptr) noexcept;
    QSslConfiguration sslConfig() const noexcept;
};

#endif  // SERVER_H

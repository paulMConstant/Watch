#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QString>
#include <QSet>
#include <QTcpServer>
#include <QHostAddress>
#include <Messages/message.h>

class QTcpSocket;

struct Client {
    explicit Client(const QString& name = "Unknown") : name(name)
    {
    }

    QString name = "Unknown";
    quint16 msgSize = 0;
};


class Server : public QObject
{
    Q_OBJECT
  public:
    Server() noexcept;
    ~Server() noexcept;

  private slots:
    void newConnection() noexcept;
    void dataReceived() noexcept;
    void clientDisconnected() noexcept;
    void socketError() noexcept;

  private:
    QTcpServer* server = new QTcpServer(this);
    QMap<QTcpSocket*, Client> connectedClients;

    void processMessage(const Message& message, QTcpSocket* source) noexcept;
    void registerClientName(const QString& name, QTcpSocket* source) noexcept;
    void sendConnectedClientsList() noexcept;
    void broadcast(const Message& message, QTcpSocket* source = nullptr) noexcept;
};

#endif // SERVER_H

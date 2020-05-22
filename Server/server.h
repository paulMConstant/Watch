#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QString>
#include <QSet>
#include <QTcpServer>
#include <QHostAddress>
#include <Messages/Messages>

class QTcpSocket;

struct Client {
    explicit Client(QTcpSocket* socket, const QString& name = "Unknown") noexcept:
        socket(socket), name(name)
    {
    }

    bool operator==(const Client& other) const
    {
        return other.socket == socket;
    }

    QTcpSocket* socket;
    QString name;
};

inline uint qHash(Client client)
{
    return qHash(client.socket);
}

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
    quint16 msgSize = 0;
    QSet<Client> connectedClients;

    void processMessage(const Message& message, QTcpSocket* source) noexcept;
    void registerClientName(const QString& name, QTcpSocket* source) noexcept;
    void sendConnectedClientsList() noexcept;
    void broadcast(const Message& message, QTcpSocket* source = nullptr) noexcept;
};

#endif // SERVER_H

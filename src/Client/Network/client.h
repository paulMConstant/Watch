#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QString>

#include <Messages/timestamp.h>
#include <Messages/message.h>

class Client : public QObject
{
    Q_OBJECT
  public:
    Client() noexcept;
    ~Client() noexcept;

    void sendTimestamp(const Timestamp& timestamp) noexcept;
    void sendChat(QString chatMessage) noexcept;
    void sendURL(const QString& URL) noexcept;
    [[nodiscard]] bool isConnectedToServer() const noexcept;

  signals:
    void connectionsChanged(QStringList);
    void timestampChanged(Timestamp);
    void urlChanged(QString);
    void disconnected();
    void connected();

  public slots:
    void connectToServer(const QString& IP) noexcept;
    void disconnectFromServer() noexcept;
    void setName(const QString& name) noexcept;

  private slots:
    void onConnected() noexcept;
    void onDisconnected() noexcept;
    void socketError() noexcept;
    void dataReceived() noexcept;

  private:
    QTcpSocket* socket = new QTcpSocket;
    quint16 msgSize = 0;
    QString name;

    void sendName() noexcept;
    void sendMessage(const Message& message) noexcept;
    void processMessage(const Message& message) noexcept;
};

#endif // CLIENT_H

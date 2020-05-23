#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QTimer>

#include <optional>

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
    [[nodiscard]] bool isConnectedToServer() const noexcept;

  signals:
    void connectionsChanged(QStringList);
    void timestampChanged(Timestamp);
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
    void sendLastUnsentTimestamp() noexcept;

  private:
    QTcpSocket* socket = new QTcpSocket;
    quint16 msgSize = 0;
    QString name;
    QTimer lastTimestampSentTimer;
    std::optional<Timestamp> lastUnsentTimestamp;

    void sendMessage(const Message& message) noexcept;
    void processMessage(const Message& message) noexcept;

    static constexpr auto minIntervalBetweenTimestamps = 50;

};

#endif // CLIENT_H

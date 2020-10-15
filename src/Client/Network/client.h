#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QSslSocket>
#include <QString>
#include <QSslError>
#include <QSslConfiguration>
#include <QList>

#include <Messages/timestamp.h>
#include <Messages/message.h>

class Client : public QObject
{
    Q_OBJECT
  public:
    Client();
    ~Client() noexcept;

    void sendTimestamp(const Timestamp& timestamp) noexcept;
    void sendChat(QString chatMessage) noexcept;
    void sendInfo(QString info) noexcept;
    void sendMedia(const QString& media) noexcept;
    [[nodiscard]] bool isConnectedToServer() const noexcept;

  signals:
    void connectionsChanged(QStringList);
    void timestampChanged(Timestamp);
    void mediaChanged(QString);
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
    void sslError(QList<QSslError> errors) noexcept;
    void dataReceived() noexcept;

  private:
    QSslSocket* socket = new QSslSocket(this);
    quint16 msgSize = 0;
    QString name;
    bool waitingForPasswordACK = false;

    QString namedMessage(QString msg) const noexcept;
    void sendMessage(const Message& message) noexcept;
    void processMessage(const Message& message) noexcept;
    QSslConfiguration sslConfig() const noexcept;
};

#endif  // CLIENT_H

#ifndef NETWORKDISPLAY_H
#define NETWORKDISPLAY_H

#include <QWidget>

class Client;

namespace Ui {
class NetworkDisplay;
}

class NetworkDisplay : public QWidget
{
    Q_OBJECT

  public:
    explicit NetworkDisplay(QWidget *parent = nullptr) noexcept;
    ~NetworkDisplay();

    void setClient(Client* client) noexcept;

  protected:
    bool eventFilter(QObject* object, QEvent* event) noexcept;

  private slots:
    void onConnectPressed() noexcept;
    void updateConnectedUsersList(const QStringList& connectedUsers) noexcept;
    void sendChatMessage() noexcept;
    void onDisconnected() noexcept;
    void onConnected() noexcept;

  private:
    Ui::NetworkDisplay *ui;
    Client* client;

    void fillMyIP();
};

#endif // NETWORKDISPLAY_H

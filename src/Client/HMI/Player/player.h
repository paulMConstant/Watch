#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>

#include <Messages/timestamp.h>
#include "Network/client.h"
#include "HMI/Player/customsignalsmediaplayer.h"

class VlcInstance;
class VlcMedia;
class Network;

namespace Ui
{
class Player;
}

class Player : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(Player)

  public:
    explicit Player(QWidget* parent = nullptr) noexcept;
    ~Player() noexcept;

    [[nodiscard]] bool hasMedia() const noexcept;

    void setClient(Client* client) noexcept;
    void goForward() noexcept;
    void goBack() noexcept;

  public slots:
    void showUI() noexcept;
    void hideUI() noexcept;
    void togglePause() noexcept;
    void askOpenFile() noexcept;
    void askOpenURL() noexcept;
    void playFile(const QString& file) noexcept;
    void shareCurrentMedia() noexcept;
    void receiveTimestamp(const Timestamp& timestamp) noexcept;

  protected:
    bool eventFilter(QObject* object, QEvent* event) noexcept;

  private slots:
    void sendTimestamp() noexcept;
    void stopVideoIfEnded() noexcept;

  private:
    Ui::Player* ui;

    VlcInstance* instance;
    VlcMedia* media;
    CustomSignalsMediaPlayer* player;

    Client* client;

    QString currentFile;

    void pause(bool sendSignal = true) noexcept;
    void play(bool sendSignal = true) noexcept;
    [[nodiscard]] bool currentFileIsLocal() noexcept;

    static constexpr auto timeJumpMS = 3000;
};

#endif  // PLAYER_H

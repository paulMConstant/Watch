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
    [[nodiscard]] const QWidget* const bottomWidget() const noexcept;
    void setClient(Client* client) noexcept;

  public slots:
    void showUI() noexcept;
    void hideUI() noexcept;
    void goForward() noexcept;
    void goBack() noexcept;
    void togglePause() noexcept;
    void askOpenFile() noexcept;
    void askOpenURL() noexcept;
    void playFile(const QString& file) noexcept;
    void shareCurrentMedia() noexcept;

  protected:
    bool eventFilter(QObject* object, QEvent* event) noexcept;

  private slots:
    void stopVideoIfEnded() noexcept;
    void updateVolume(int volume) noexcept;
    void sendTimestamp() noexcept;
    void receiveTimestamp(const Timestamp& timestamp) noexcept;
    void receiveDistantMedia(const QString& media) noexcept;

  private:
    Ui::Player* ui;

    VlcInstance* instance;
    VlcMedia* media;
    CustomSignalsMediaPlayer* player;

    Client* client;

    QString currentFile;

    void pause(bool sendSignal = true) noexcept;
    void play(bool sendSignal = true) noexcept;
    void setPauseIcon() noexcept;
    [[nodiscard]] bool currentFileIsLocal() noexcept;

    static constexpr auto timeJumpMS = 3000;
};

#endif  // PLAYER_H

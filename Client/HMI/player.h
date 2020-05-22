#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>

#include <VLCQtCore/MediaPlayer.h>

#include "Network/client.h"
#include <Messages/Messages>

class VlcInstance;
class VlcMedia;
class Network;

namespace Ui {
class Player;
}

class CustomVlcMediaPlayer : public VlcMediaPlayer
{
    Q_OBJECT
  public:
    CustomVlcMediaPlayer(VlcInstance *instance) : VlcMediaPlayer(instance)
    {
    }
    /*!
     * \brief updates the time without sending a signal.
     */
    void setReceivedTime(int timeMS)
    {
        VlcMediaPlayer::setTime(timeMS);
    }

  signals:
    void timeManuallyChanged(int);

  public slots:
    /*!
     * \brief updates the time and sends a signal.
     */
    void setTime(int timeMS)
    {
        VlcMediaPlayer::setTime(timeMS);
        emit timeManuallyChanged(timeMS);
    }

};

class Player : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(Player)
  public:
    explicit Player(QWidget *parent = nullptr) noexcept;
    ~Player() noexcept;

    void setClient(Client* client) noexcept;
    void goForward() noexcept;
    void goBack() noexcept;

  public slots:
    void showUI() noexcept;
    void hideUI() noexcept;
    void togglePause() noexcept;
    void playFile(const QString& file) noexcept;
    void receiveStream(const Timestamp& timestamp) noexcept;

  protected:
    bool eventFilter(QObject* object, QEvent* event) noexcept;

  private slots:
    void sendTimestamp() noexcept;

  private:
    Ui::Player* ui;

    VlcInstance* instance;
    VlcMedia* media;
    CustomVlcMediaPlayer* player;

    Client* client;

    void pause() noexcept;
    void play() noexcept;
};

#endif // PLAYER_H

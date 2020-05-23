#ifndef CUSTOMSIGNALSMEDIAPLAYER_H
#define CUSTOMSIGNALSMEDIAPLAYER_H

#include <QObject>
#include <VLCQtCore/MediaPlayer.h>

class VlcInstance;
class VlcMedia;

class CustomSignalsMediaPlayer : public VlcMediaPlayer
{
    Q_OBJECT
  public:
    CustomSignalsMediaPlayer(VlcInstance *instance);
    /*!
     * \brief returns true if the player is paused else false.
     * This updates faster than the Vlc state() method
     * (which can send wrong info to the server).
     */
    [[nodiscard]] bool isPaused() const noexcept;

    /*!
     * \brief utility method which returns true if a video is loaded and not about to end.
     */
    [[nodiscard]] bool videoIsOn() const noexcept;

    /*!
     * \brief updates the time without sending a signal.
     */
    void noSignalSetTime(int timeMS);
    /*!
     * \brief plays and does not send a signal.
     */
    void noSignalPlay();
    /*!
     * \brief pauses and does not send a signal.
     */
    void noSignalPause();

  signals:
    void manualActionTriggered();

  public slots:
    /*!
     * \brief updates the time and sends a signal.
     */
    void setTime(int timeMS);
    /*!
     * \brief plays ans sends a signal.
     */
    void play();
    /*!
     * \brief pauses and sends a signal.
     */
    void pause();

    /*!
     * \brief opens the media and plays it.
     */
    void open(VlcMedia* media);

  private:
    bool paused = true;
};

#endif // CUSTOMSIGNALSMEDIAPLAYER_H

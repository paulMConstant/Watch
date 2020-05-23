#include "customsignalsmediaplayer.h"

#include <VLCQtCore/Media.h>

CustomSignalsMediaPlayer::CustomSignalsMediaPlayer(VlcInstance *instance) :
    VlcMediaPlayer(instance)
{
}

bool CustomSignalsMediaPlayer::isPaused() const noexcept
{
    return paused;
}

bool CustomSignalsMediaPlayer::videoIsPlayable() const noexcept
{
    return timeIsPlayable(time());
}

bool CustomSignalsMediaPlayer::timeIsPlayable(int timeMS) const noexcept
{
    constexpr auto msToEnd = 300;
    return (currentMedia() != nullptr && (timeMS < currentMedia()->duration() - msToEnd));
}

void CustomSignalsMediaPlayer::noSignalSetTime(int timeMS)
{
    VlcMediaPlayer::setTime(timeMS);
}

void CustomSignalsMediaPlayer::noSignalPlay()
{
    VlcMediaPlayer::play();
    paused = false;
}

void CustomSignalsMediaPlayer::noSignalPause()
{
    VlcMediaPlayer::pause();
    paused = true;
}

void CustomSignalsMediaPlayer::setTime(int timeMS)
{
    if (timeIsPlayable(timeMS))
    {
        noSignalSetTime(timeMS);
        emit manualActionTriggered();
    }
}

void CustomSignalsMediaPlayer::play()
{
    noSignalPlay();
    emit manualActionTriggered();
}

void CustomSignalsMediaPlayer::pause()
{
    noSignalPause();
    emit manualActionTriggered();
}

void CustomSignalsMediaPlayer::open(VlcMedia* media)
{
    VlcMediaPlayer::open(media);
    paused = false;
}

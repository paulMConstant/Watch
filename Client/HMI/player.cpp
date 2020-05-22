#include "player.h"
#include "ui_player.h"

#include <VLCQtCore/Common.h>
#include <VLCQtCore/Instance.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/Enums.h>

#include <QMouseEvent>
#include <QFileInfo>

#include "Network/client.h"

Player::Player(QWidget *parent) noexcept :
    QWidget(parent),
    ui(new Ui::Player)
{
    ui->setupUi(this);
    instance = new VlcInstance(VlcCommon::args(), this);
    player = new CustomVlcMediaPlayer(instance);
    player->setVideoWidget(ui->video);
    ui->video->installEventFilter(this);

    ui->video->setMediaPlayer(player);
    ui->volume->setMediaPlayer(player);
    ui->volume->setVolume(50);
    ui->seek->setMediaPlayer(player);

    connect(ui->playPause, SIGNAL(clicked(bool)), this, SLOT(togglePause()));
    connect(player, SIGNAL(timeManuallyChanged(int)), this, SLOT(sendTimestamp()));
    connect(player, SIGNAL(paused()), this, SLOT(sendTimestamp()));
    connect(player, SIGNAL(playing()), this, SLOT(sendTimestamp()));
}

Player::~Player() noexcept
{
    delete player;
    delete instance;
    delete media;
    delete ui;
}

void Player::setClient(Client* client) noexcept
{
    this->client = client;
    connect(client, SIGNAL(timestampChanged(Timestamp)), this, SLOT(receiveStream(Timestamp)));
}

void Player::goForward() noexcept
{
    if (player->currentMedia() == nullptr)
    {
        return;
    }
    if (player->currentMedia()->duration() - player->time() > 4000)
    {
        player->setTime(player->time() + 2000);
    }
}

void Player::goBack() noexcept
{
    if (player->time() > 2000)
    {
        player->setTime(player->time() - 2000);
    }
}

void Player::hideUI() noexcept
{
    ui->bottomWidget->hide();
    setCursor(Qt::BlankCursor);
}

void Player::showUI() noexcept
{
    ui->bottomWidget->show();
    setCursor(Qt::ArrowCursor);
}

void Player::togglePause() noexcept
{
    if (player->currentMedia() == nullptr)
    {
        return;
    }
    if (player->state() == Vlc::Paused)
    {
        play();
    }
    else
    {
        pause();
    }
}

void Player::playFile(const QString& file) noexcept
{
    media = new VlcMedia(file, true, instance);
    player->open(media);
    play();
    client->sendChat("<i>playing '" + QFileInfo(file).fileName() + "'.</i>");
}

void Player::sendTimestamp() noexcept
{
    auto timestamp = Timestamp(player->state() == Vlc::Paused, player->time());
    client->sendTimestamp(timestamp);
}

void Player::receiveStream(const Timestamp &timestamp) noexcept
{
    if (player->currentMedia() == nullptr)
    {
        return;
    }

    player->setReceivedTime(timestamp.timeMS);

    if (timestamp.paused)
    {
        if (player->state() != Vlc::State::Paused)
        {
            pause();
        }
    }
    else
    {
        if (player->state() != Vlc::State::Playing)
        {
            play();
        }
    }
}

void Player::pause() noexcept
{
    if (player->currentMedia() == nullptr || (player->state() != Vlc::State::Playing
                                              && player->state() != Vlc::State::Paused))
    {
        return;
    }
    player->pause();
    ui->playPause->setText("Play");
}

void Player::play() noexcept
{
    if (player->currentMedia() == nullptr || (player->state() != Vlc::State::Playing
                                              && player->state() != Vlc::State::Paused))
    {
        return;
    }
    player->play();
    ui->playPause->setText("Pause");
}

bool Player::eventFilter(QObject* object, QEvent* event) noexcept
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        auto mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton)
        {
            togglePause();
            return true;
        }
        return false;
    }
    return QObject::eventFilter(object, event);
}

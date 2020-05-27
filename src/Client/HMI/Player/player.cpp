#include "HMI/Player/player.h"

#include <QMouseEvent>
#include <QFileInfo>
#include <QFileDialog>
#include <QInputDialog>
#include <QFile>

#include <VLCQtCore/Common.h>
#include <VLCQtCore/Instance.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/Enums.h>

#include "Network/client.h"
#include "Logger/logger.h"

#include "ui_player.h"  // NOLINT[build/include_subdir]

Player::Player(QWidget* parent) noexcept :
    QWidget(parent),
    ui(new Ui::Player)
{
    ui->setupUi(this);
    instance = new VlcInstance(VlcCommon::args(), this);
    player = new CustomSignalsMediaPlayer(instance);
    player->setVideoWidget(ui->video);
    ui->video->installEventFilter(this);

    ui->video->setMediaPlayer(player);
    ui->volume->setMediaPlayer(player);
    ui->volume->setVolume(50);
    ui->seek->setMediaPlayer(player);
    connect(ui->volume, SIGNAL(newVolume(int)), this, SLOT(updateVolumeIcon(int)));
    connect(ui->playPause, SIGNAL(clicked(bool)), this, SLOT(togglePause()));
    connect(player, SIGNAL(manualActionTriggered()), this, SLOT(sendTimestamp()));
    connect(player, SIGNAL(timeChanged(int)), this, SLOT(stopVideoIfEnded()));
}

Player::~Player() noexcept
{
    delete player;
    delete media;
    delete instance;
    delete ui;
}

bool Player::hasMedia() const noexcept
{
    return player->currentMedia() != nullptr;
}

void Player::setClient(Client* client) noexcept
{
    this->client = client;
    connect(client, SIGNAL(timestampChanged(Timestamp)), this, SLOT(receiveTimestamp(Timestamp)));
    connect(client, SIGNAL(mediaChanged(QString)), this, SLOT(receiveDistantMedia(QString)));
}

void Player::goForward() noexcept
{
    if (player->videoIsPlayable() == false)
    {
        return;
    }
    constexpr auto securityMarginMS = 1000;
    const auto timeLeft = player->currentMedia()->duration() - player->time();
    if (timeLeft > timeJumpMS + securityMarginMS)
    {
        player->setTime(player->time() + timeJumpMS);
    }
}

void Player::goBack() noexcept
{
    if (player->time() > timeJumpMS)
    {
        player->setTime(player->time() - timeJumpMS);
    }
}

void Player::askOpenFile() noexcept
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open file"),
                   QDir::homePath(),
                   tr("Multimedia files(*)"));

    if (file.isEmpty())
    {
        return;
    }

    playFile(file);
}

void Player::askOpenURL() noexcept
{
    QString url =
        QInputDialog::getText(this, tr("Open Url"), tr("Enter the URL you want to play"));

    if (url.isEmpty())
    {
        return;
    }

    playFile(url);
}

void Player::shareCurrentMedia() noexcept
{
    if (player->currentMedia() == nullptr)
    {
        Logger::printRed("Cannot share media : no open media.");
        return;
    }

    if (currentFileIsLocal())
    {
        Logger::printRed("Sharing of local files is not yet implemented.");
        return;
    }
    client->sendMedia(currentFile);
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
    if (player->videoIsPlayable() == false)
    {
        return;
    }
    if (player->isPaused())
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
    currentFile = file;
    auto local = currentFileIsLocal();
    media = new VlcMedia(currentFile, local, instance);
    player->open(media);
    ui->playPause->setIcon(QIcon(":/icons/pause"));
    auto filename = currentFile;
    if (local)
    {
        filename = QFileInfo(currentFile).fileName();
    }
    client->sendInfo("playing '" + filename + "'.");
}

bool Player::currentFileIsLocal() noexcept
{
    return QFile::exists(currentFile);
}

void Player::updateVolumeIcon(int volume) noexcept
{
    auto iconPath = QString();
    if (volume == 0)
    {
        iconPath = ":/icons/volume0";
    }
    else if (volume < 33)
    {
        iconPath = ":/icons/volume1";
    }
    else if (volume < 67)
    {
        iconPath = ":/icons/volume2";
    }
    else
    {
        iconPath = ":/icons/volume3";
    }
    ui->volumeIcon->setPixmap(QPixmap(iconPath));
}

void Player::sendTimestamp() noexcept
{
    auto timestamp = Timestamp(player->isPaused(), player->time());
    client->sendTimestamp(timestamp);
}

void Player::receiveTimestamp(const Timestamp& timestamp) noexcept
{
    if (hasMedia() == false)
    {
        return;
    }

    player->noSignalSetTime(timestamp.timeMS);

    if (timestamp.paused)
    {
        pause(false);
    }
    else
    {
        play(false);
    }
}

void Player::receiveDistantMedia(const QString& media) noexcept
{
    Logger::printGreen("Received media to play : '" + media + '\'');
    playFile(media);
}

void Player::pause(bool sendSignal) noexcept
{
    if (hasMedia() == false)
    {
        return;
    }
    if (sendSignal)
    {
        player->pause();
    }
    else
    {
        player->noSignalPause();
    }
    ui->playPause->setIcon(QIcon(":/icons/play"));
}

void Player::play(bool sendSignal) noexcept
{
    if (player->videoIsPlayable() == false)
    {
        return;
    }
    if (sendSignal)
    {
        player->play();
    }
    else
    {
        player->noSignalPlay();
    }
    ui->playPause->setIcon(QIcon(":/icons/pause"));
}

void Player::stopVideoIfEnded() noexcept
{
    if (player->videoIsPlayable() == false)
    {
        pause(false);
    }
}

bool Player::eventFilter(QObject* object, QEvent* event) noexcept
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        auto mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton && hasMedia())
        {
            togglePause();
            return true;
        }
        return false;
    }
    return QObject::eventFilter(object, event);
}

#include "HMI/mainwindow.h"

#include <QKeyEvent>
#include <QLineEdit>

#include "ui_mainwindow.h"  // NOLINT[build/include_subdir]
#include "HMI/Player/player.h"

MainWindow::MainWindow(QWidget* parent) noexcept :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initNetwork();
    initSlotConnections();

    qApp->installEventFilter(this);
}

MainWindow::~MainWindow() noexcept
{
    delete ui;
}

bool MainWindow::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        const auto keyEvent = static_cast<QKeyEvent*>(event);
        const auto focus = QApplication::focusWidget();
        const auto noLineEditInFocus = (focus == nullptr ||
                                        qobject_cast<QLineEdit*>(focus) == nullptr);

        if (keyEvent->key() == Qt::Key_Space)
        {
            if (noLineEditInFocus)
            {
                ui->player->togglePause();
                return true;
            }
            return false;
        }
        if (keyEvent->key() == Qt::Key_F11)
        {
            if (ui->menuBar->isHidden())
            {
                toggleFullscreen();
            }
            return false;
        }
        if (keyEvent->key() == Qt::Key_Escape)
        {
            if (isFullScreen())
            {
                toggleFullscreen();
            }
            return false;
        }
        if (keyEvent->key() == Qt::Key_Left)
        {
            if (noLineEditInFocus)
            {
                ui->player->goBack();
                return true;
            }
            return false;
        }
        if (keyEvent->key() == Qt::Key_Right)
        {
            if (noLineEditInFocus)
            {
                ui->player->goForward();
                return true;
            }
            return false;
        }
    }
    if (event->type() == QEvent::MouseMove)
    {
        showUI();
        if (isFullScreen() && ui->player->underMouse())
        {
            hideUITimer.start(hideDelayMS);
        }
        return false;
    }
    return QObject::eventFilter(object, event);
}

void MainWindow::showUI() noexcept
{
    ui->menuBar->show();
    ui->networkDock->show();
    ui->player->showUI();
    hideUITimer.stop();
}

void MainWindow::hideUI() noexcept
{
    if (ui->player->hasMedia() == false)
    {
        return;
    }
    ui->menuBar->hide();
    if (ui->networkDock->isFloating() == false)
    {
        ui->networkDock->hide();
    }
    ui->player->hideUI();
}

void MainWindow::toggleFullscreen() noexcept
{
    if (isFullScreen())
    {
        showUI();
        showNormal();
    }
    else
    {
        hideUI();
        showFullScreen();
    }
}

void MainWindow::initNetwork() noexcept
{
    ui->networkDisplay->setClient(&client);
    ui->player->setClient(&client);
}

void MainWindow::initSlotConnections() noexcept
{
    connect(ui->actionFullscreen, SIGNAL(triggered(bool)),
            this, SLOT(toggleFullscreen()));

    connect(&hideUITimer, SIGNAL(timeout()), this, SLOT(hideUI()));
    connect(ui->actionOpen_local, SIGNAL(triggered(bool)), ui->player, SLOT(askOpenFile()));
    connect(ui->actionOpen_url, SIGNAL(triggered(bool)), ui->player, SLOT(askOpenURL()));
    connect(ui->actionShare_current, SIGNAL(triggered(bool)),
            ui->player, SLOT(shareCurrentMedia()));
}

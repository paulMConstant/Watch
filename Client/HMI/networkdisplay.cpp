#include "networkdisplay.h"
#include "ui_networkdisplay.h"

#include <QSettings>

#include "Network/client.h"
#include "Logger/logger.h"

NetworkDisplay::NetworkDisplay(QWidget *parent) noexcept:
    QWidget(parent),
    ui(new Ui::NetworkDisplay)
{
    ui->setupUi(this);
    Logger::init(ui->messageList);
    connect(ui->IPline, SIGNAL(returnPressed()), SLOT(setFocus()));
    connect(ui->nameLine, SIGNAL(returnPressed()), SLOT(setFocus()));
    qApp->installEventFilter(this);

    QSettings settings("Paul Constant", "Watch");
    if (settings.allKeys().contains("Name"))
    {
        ui->nameLine->setText(settings.value("Name").toString());
    }
}

NetworkDisplay::~NetworkDisplay()
{
    QSettings settings("Paul Constant", "Watch");
    settings.setValue("Name", ui->nameLine->text());
    delete ui;
}

void NetworkDisplay::setClient(Client *client) noexcept
{
    this->client = client;
    connect(ui->connectButton, SIGNAL(clicked(bool)), this, SLOT(onConnectPressed()));
    connect(ui->IPline, SIGNAL(returnPressed()), this, SLOT(setFocus()));
    connect(ui->nameLine, SIGNAL(textChanged(QString)), client, SLOT(setName(QString)));
    connect(ui->messageLine, SIGNAL(returnPressed()), this, SLOT(sendChatMessage()));

    connect(client, SIGNAL(connectionsChanged(QStringList)),
            this, SLOT(updateConnectedUsersList(QStringList)));
    connect(client, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(client, SIGNAL(connected()), this, SLOT(onConnected()));

    client->setName(ui->nameLine->text());
}

void NetworkDisplay::onDisconnected() noexcept
{
    ui->connectedUsersList->clear();
    ui->IPline->setEnabled(true);
    ui->IPlabel->setEnabled(true);
    ui->connectButton->setText("Connect");
}

void NetworkDisplay::onConnected() noexcept
{
    ui->IPline->setEnabled(false);
    ui->IPlabel->setEnabled(false);
    ui->connectButton->setText("Disconnect");
}

void NetworkDisplay::onConnectPressed() noexcept
{
    if (client->isConnectedToServer())
    {
        client->disconnectFromServer();
    }
    else
    {
        client->connectToServer(ui->IPline->text());
    }
}

void NetworkDisplay::updateConnectedUsersList(const QStringList& connectedUsers) noexcept
{
    ui->connectedUsersList->clear();
    for (const auto& user : connectedUsers)
    {
        ui->connectedUsersList->addItem(user);
    }
}

void NetworkDisplay::sendChatMessage() noexcept
{
    auto text = ui->messageLine->text();
    text.replace('<', "&lt;");
    text.replace('>', "&gt;");
    client->sendChat(text);
    ui->messageLine->clear();
}

/*!
 * \brief Clears the focus of the QLineEdits on mouse press
 */
bool NetworkDisplay::eventFilter(QObject* object, QEvent* event) noexcept
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        if (ui->IPline->underMouse() == false)
        {
            ui->IPline->clearFocus();
        }
        if (ui->nameLine->underMouse() == false)
        {
            ui->nameLine->clearFocus();
        }
        return false;
    }
    return QObject::eventFilter(object, event);
}

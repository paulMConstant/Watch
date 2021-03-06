#include "HMI/networkdisplay.h"

#include <QSettings>
#include <QtGlobal> 

#include "ui_networkdisplay.h"  // NOLINT[build/include_subdir]
#include "Network/client.h"
#include "Logger/logger.h"
#include "Helpers/helpers.h"
#include "Global/global.h"

NetworkDisplay::NetworkDisplay(QWidget* parent) noexcept:
    QWidget(parent),
    ui(new Ui::NetworkDisplay)
{
    ui->setupUi(this);
    Logger::init(ui->messageList);
    connect(ui->serverLine, &QLineEdit::returnPressed, this, qOverload<>(&QWidget::setFocus));
    connect(ui->nameLine, &QLineEdit::returnPressed, this, qOverload<>(&QWidget::setFocus));
    qApp->installEventFilter(this);

    QSettings settings(Global::organizationName, Global::appName);
    if (settings.allKeys().contains(userNameSettingsKey))
    {
        ui->nameLine->setText(settings.value(userNameSettingsKey).toString());
    }
    if (settings.allKeys().contains(serverNameSettingsKey))
    {
        ui->serverLine->setText(settings.value(serverNameSettingsKey).toString());
    }
}

NetworkDisplay::~NetworkDisplay()
{
    QSettings settings(Global::organizationName, Global::appName);
    settings.setValue(userNameSettingsKey, ui->nameLine->text());
    settings.setValue(serverNameSettingsKey, ui->serverLine->text());
    delete ui;
}

void NetworkDisplay::setClient(Client* client) noexcept
{
    this->client = client;
    connect(ui->connectButton, &QPushButton::clicked, this, &NetworkDisplay::onConnectPressed);
    connect(ui->serverLine, &QLineEdit::returnPressed, this, qOverload<>(&QWidget::setFocus));
    connect(ui->nameLine, &QLineEdit::editingFinished, this, &NetworkDisplay::changeName);
    connect(ui->messageLine, &QLineEdit::returnPressed, this, &NetworkDisplay::sendChatMessage);

    connect(client, &Client::connectionsChanged,
            this, &NetworkDisplay::updateConnectedUsersList);
    connect(client, &Client::disconnected, this, &NetworkDisplay::onDisconnected);
    connect(client, &Client::connected, this, &NetworkDisplay::onConnected);
    connect(client, &Client::connecting, this, &NetworkDisplay::onConnecting);

    changeName();
}

void NetworkDisplay::changeName() noexcept
{
    auto name = ui->nameLine->text();
    name = name.trimmed();
    ui->nameLine->setText(name);
    client->setName(name);
}

void NetworkDisplay::onDisconnected() noexcept
{
    ui->connectedUsersList->clear();
    ui->serverLine->setEnabled(true);
    ui->serverLabel->setEnabled(true);
    ui->nameLine->setEnabled(true);
    ui->nameLabel->setEnabled(true);
    ui->connectButton->setText("Connect");
}

void NetworkDisplay::onConnected() noexcept
{
    ui->serverLine->setEnabled(false);
    ui->serverLabel->setEnabled(false);
    ui->nameLine->setEnabled(false);
    ui->nameLabel->setEnabled(false);
    ui->connectButton->setText("Disconnect");
}

void NetworkDisplay::onConnecting() noexcept
{
    ui->serverLine->setEnabled(false);
    ui->serverLabel->setEnabled(false);
    ui->nameLine->setEnabled(false);
    ui->nameLabel->setEnabled(false);
    ui->connectButton->setText("Cancel");
}

void NetworkDisplay::onConnectPressed() noexcept
{
    if (client->isConnectedToServer())
    {
        client->disconnectFromServer();
    }
    else if (client->isConnectingToServer())
    {
        client->cancelConnecting();
        onDisconnected();
    }
    else
    {
        client->connectToServer(ui->serverLine->text());
    }
}

void NetworkDisplay::updateConnectedUsersList(const QStringList& connectedUsers) noexcept
{
    if (ui->connectedUsersList->count() == 0)
    {
        // We just joined the room
        lastConnectedUsers = connectedUsers;
    }
    else
    {
        printUserListChanges(connectedUsers);
    }

    ui->connectedUsersList->clear();
    for (const auto& user : connectedUsers)
    {
        ui->connectedUsersList->addItem(user);
    }
}

void NetworkDisplay::printUserListChanges(const QStringList& newConnectedUsers) noexcept
{
    for (const auto& user : newConnectedUsers)
    {
        if (lastConnectedUsers.contains(user) == false)
        {
            Logger::printBlack("<i>" + Helpers::String::cleanHtml(user) + " joined the room.</i>");
        }
        else
        {
            lastConnectedUsers.removeOne(user);
        }
    }
    for (const auto& user : lastConnectedUsers)
    {
        Logger::printBlack("<i>" + user + " left.</i>");
    }
    lastConnectedUsers = newConnectedUsers;
}

void NetworkDisplay::sendChatMessage() noexcept
{
    const auto text = Helpers::String::cleanHtml(ui->messageLine->text());
    if (text.isEmpty())
    {
        return;
    }
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
        if (ui->serverLine->underMouse() == false)
        {
            ui->serverLine->clearFocus();
        }
        if (ui->nameLine->underMouse() == false)
        {
            ui->nameLine->clearFocus();
        }
        return false;
    }
    return QObject::eventFilter(object, event);
}

#include "HMI/networkdisplay.h"

#include <QSettings>

#include "ui_networkdisplay.h"  // NOLINT[build/include_subdir]
#include "Network/client.h"
#include "Logger/logger.h"
#include "Helpers/helpers.h"

NetworkDisplay::NetworkDisplay(QWidget* parent) noexcept:
    QWidget(parent),
    ui(new Ui::NetworkDisplay)
{
    ui->setupUi(this);
    Logger::init(ui->messageList);
    connect(ui->serverLine, SIGNAL(returnPressed()), SLOT(setFocus()));
    connect(ui->nameLine, SIGNAL(returnPressed()), SLOT(setFocus()));
    qApp->installEventFilter(this);

    QSettings settings("Paul Constant", "Watch");
    if (settings.allKeys().contains("Name"))
    {
        ui->nameLine->setText(settings.value("Name").toString());
    }
    if (settings.allKeys().contains("Server"))
    {
        ui->serverLine->setText(settings.value("Server").toString());
    }
}

NetworkDisplay::~NetworkDisplay()
{
    QSettings settings("Paul Constant", "Watch");
    settings.setValue("Name", ui->nameLine->text());
    settings.setValue("Server", ui->serverLine->text());
    delete ui;
}

void NetworkDisplay::setClient(Client* client) noexcept
{
    this->client = client;
    connect(ui->connectButton, SIGNAL(clicked(bool)), this, SLOT(onConnectPressed()));
    connect(ui->serverLine, SIGNAL(returnPressed()), this, SLOT(setFocus()));
    connect(ui->nameLine, SIGNAL(editingFinished()), this, SLOT(changeName()));
    connect(ui->messageLine, SIGNAL(returnPressed()), this, SLOT(sendChatMessage()));

    connect(client, SIGNAL(connectionsChanged(QStringList)),
            this, SLOT(updateConnectedUsersList(QStringList)));
    connect(client, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(client, SIGNAL(connected()), this, SLOT(onConnected()));

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

void NetworkDisplay::onConnectPressed() noexcept
{
    if (client->isConnectedToServer())
    {
        client->disconnectFromServer();
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
            Logger::printChatMsg("<i>" + user + " joined the room.</i>");
        }
        else
        {
            lastConnectedUsers.removeOne(user);
        }
    }
    for (const auto& user : lastConnectedUsers)
    {
        Logger::printChatMsg("<i>" + user + " left.</i>");
    }
    lastConnectedUsers = newConnectedUsers;
}

void NetworkDisplay::sendChatMessage() noexcept
{
    const auto text = Helpers::String::cleanHtml(ui->messageLine->text());
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

#include <QCoreApplication>
#include <QMetaType>

#include <Messages/message.h>
#include "Server/server.h"

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);
    Message::registerMetatypes();
    Server server;
    return a.exec();
}

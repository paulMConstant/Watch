#include <QCoreApplication>
#include <QMetaType>

#include <Messages/timestamp.h>
#include "Server/server.h"

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);
    qRegisterMetaType<Timestamp>("Timestamp");
    qRegisterMetaTypeStreamOperators<Timestamp>("Timestamp");
    auto server = Server();
    return a.exec();
}

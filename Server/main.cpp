#include <QCoreApplication>
#include <QMetaType>

#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qRegisterMetaType<Timestamp>("Timestamp");
    qRegisterMetaTypeStreamOperators<Timestamp>("Timestamp");
    auto server = Server();
    return a.exec();
}

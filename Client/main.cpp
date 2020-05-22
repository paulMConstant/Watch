#include "HMI/mainwindow.h"

#include <QApplication>
#include <VLCQtCore/Common.h>
#include <QMetaType>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VlcCommon::setPluginPath(a.applicationDirPath() + "/plugins");
    qRegisterMetaType<Timestamp>("Timestamp");
    qRegisterMetaTypeStreamOperators<Timestamp>("Timestamp");
    MainWindow w;
    w.show();

    return a.exec();
}

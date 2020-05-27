#include <QApplication>
#include <VLCQtCore/Common.h>

#include <Messages/message.h>
#include "HMI/mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    VlcCommon::setPluginPath(a.applicationDirPath() + "/plugins");
    Message::registerMetatypes();
    MainWindow w;
    w.show();

    return a.exec();
}

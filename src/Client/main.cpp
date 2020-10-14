#include <QApplication>
#include <VLCQtCore/Common.h>

#include <Messages/message.h>

#include "HMI/mainwindow.h"
#include "Global/global.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    Global::setupIcons();
    VlcCommon::setPluginPath(a.applicationDirPath() + "/plugins");
    Message::registerMetatypes();
    MainWindow w;
    w.show();

    return a.exec();
}

#include "global.h"

#include <QGuiApplication>
#include <QIcon>

void Global::setupIcons() noexcept
{
    // HD icons on every platform
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#ifdef _WIN32
    QGuiApplication::setWindowIcon(QIcon(":/icons/logo.ico"));
#endif
#ifdef linux
    QGuiApplication::setWindowIcon(QIcon(":/icons/logo.png"));
#endif
}

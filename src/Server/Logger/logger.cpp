#include "Logger/logger.h"

#include <QDebug>
#include <QTime>

void Logger::print(const QString& msg) noexcept
{
    const auto timeStamp = '[' + QTime::currentTime().toString("hh:mm:ss") + "]";
    qDebug() << timeStamp + msg;
}

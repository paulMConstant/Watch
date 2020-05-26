#include "Logger/logger.h"

#include <QTime>
#include <QTextStream>

void Logger::print(const QString& msg) noexcept
{
    auto out = QTextStream(stdout);
    const auto timeStamp = '[' + QTime::currentTime().toString("hh:mm:ss") + "] ";
    out << timeStamp << msg;
    Qt::endl(out);
}

#include "Logger/logger.h"

#include <QTime>
#include <QTextStream>
#include <QDir>

void Logger::print(const QString& msg) noexcept
{
    auto outConsole = QTextStream(stdout);
    const auto timestamp = '[' + QTime::currentTime().toString("hh:mm:ss") + "] ";
    outConsole << timestamp << msg;
    Qt::endl(outConsole);

    auto logFile = QFile(QDir::current().path() + "/logs.txt");
    logFile.open(QIODevice::Append);
    auto outFile = QTextStream(&logFile);
    outFile << timestamp << msg;
    Qt::endl(outFile);
}

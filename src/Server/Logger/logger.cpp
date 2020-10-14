#include "Logger/logger.h"

#include <QTime>
#include <QTextStream>
#include <QDir>

void Logger::print(const QString& msg) noexcept
{
    QTextStream outConsole(stdout); 
    const auto timestamp = '[' + QTime::currentTime().toString("hh:mm:ss") + "] ";
    outConsole << timestamp << msg;
    Qt::endl(outConsole);

    QFile logFile(QDir::current().path() + "/logs.txt");
    logFile.open(QIODevice::Append);
    QTextStream outFile(&logFile);
    outFile << timestamp << msg;
    Qt::endl(outFile);
}

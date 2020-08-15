#include "Logger/logger.h"

#include <QDebug>
#include <QTime>
#include <QTextCursor>

namespace
{
QTextEdit* msgList = nullptr;

void print(const QString& msg, const QColor& color) noexcept
{
    if (msgList == nullptr)
    {
        qDebug() << "Logger should be initialized !";
        return;
    }
    const auto timeStamp = '[' + QTime::currentTime().toString("hh:mm:ss") + "] ";
    msgList->moveCursor(QTextCursor::End);
    msgList->insertHtml("<font color=\"" + color.name() + "\">" + timeStamp + msg + "</font><br>");
}
}  // namespace

void Logger::init(QTextEdit* outputList) noexcept
{
    msgList = outputList;
}

void Logger::printRed(const QString& msg) noexcept
{
    print(msg, Qt::red);
}

void Logger::printGreen(const QString& msg) noexcept
{
    print(msg, Qt::darkGreen);
}

void Logger::printBlack(const QString& msg) noexcept
{
    print(msg, Qt::black);
}

#ifndef LOGGER_H
#define LOGGER_H

#include <QTextEdit>
#include <QString>

namespace Logger
{
    void init(QTextEdit* outputList) noexcept;
    void printInfo(const QString& msg) noexcept;
    void printError(const QString& msg) noexcept;
    void printChatMsg(const QString& msg) noexcept;
} // namespace

#endif // LOGGER_H

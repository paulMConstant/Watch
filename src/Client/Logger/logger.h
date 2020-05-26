#ifndef LOGGER_H
#define LOGGER_H

#include <QTextEdit>
#include <QString>

namespace Logger
{
void init(QTextEdit* outputList) noexcept;
void printGreen(const QString& msg) noexcept;
void printRed(const QString& msg) noexcept;
void printBlack(const QString& msg) noexcept;
}  // namespace Logger

#endif  // LOGGER_H

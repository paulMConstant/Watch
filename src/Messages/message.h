#ifndef MESSAGE_H
#define MESSAGE_H

#include <QDataStream>
#include <QVariant>
#include <QByteArray>

struct Message
{
    enum class Type {Invalid, Name, Timestamp, Chat};

    Message() = default;
    explicit Message(Message::Type type, QVariant data) noexcept;
    QByteArray toByteArray() const noexcept;

    Message::Type type;
    QVariant data;
};

QDataStream& operator<<(QDataStream& ds, const Message& message);
QDataStream& operator>>(QDataStream& ds, Message& message);

#endif // MESSAGE_H

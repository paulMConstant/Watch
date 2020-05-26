#ifndef MESSAGE_H
#define MESSAGE_H

#include <QDataStream>
#include <QVariant>
#include <QByteArray>

struct Message
{
    enum class Type {Hello, Names, Timestamp, Chat, Media, Info};

    Message() = default;
    explicit Message(Message::Type type, QVariant data) noexcept;
    QByteArray toByteArray() const noexcept;

    Message::Type type;
    QVariant data;

    static void registerMetatypes() noexcept;
};

QDataStream& operator<<(QDataStream& ds, const Message& message);
QDataStream& operator>>(QDataStream& ds, Message& message);

#endif  // MESSAGE_H

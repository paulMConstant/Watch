#include "message.h"

Message::Message(Message::Type type, QVariant data) noexcept:
    type(type), data(data)
{
}

QByteArray Message::toByteArray() const noexcept
{
    auto packet = QByteArray();
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << static_cast<quint16>(0);
    out << *this;
    out.device()->seek(0);
    out << static_cast<quint16>(packet.size() - sizeof(quint16));
    return packet;
}

QDataStream& operator<<(QDataStream& ds, const Message& message)
{
    ds << static_cast<int>(message.type);
    ds << message.data;
    return ds;
}

QDataStream& operator>>(QDataStream& ds, Message& message)
{
    auto type = 0;
    ds >> type;
    message.type = Message::Type(type);
    ds >> message.data;
    return ds;
}

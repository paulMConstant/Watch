#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <QDataStream>

struct Timestamp
{
    Timestamp() = default;
    explicit Timestamp(bool paused, int timeMS) noexcept;
    bool paused;
    int timeMS;
};
Q_DECLARE_METATYPE(Timestamp)

QDataStream& operator<<(QDataStream& ds, const Timestamp& timestamp);
QDataStream& operator>>(QDataStream& ds, Timestamp& timestamp);

#endif  // TIMESTAMP_H

#include "timestamp.h"

Timestamp::Timestamp(bool paused, int timeMS) noexcept:
    paused(paused), timeMS(timeMS)
{
}

QDataStream& operator<<(QDataStream& ds, const Timestamp& timestamp)
{
    ds << timestamp.paused;
    ds << timestamp.timeMS;
    return ds;
}

QDataStream& operator>>(QDataStream& ds, Timestamp& timestamp)
{
    ds >> timestamp.paused;
    ds >> timestamp.timeMS;
    return ds;
}

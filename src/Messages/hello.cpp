#include "hello.h"  // NOLINT[build/include_subdir]

Hello::Hello(const QByteArray& password, const QString& name) noexcept:
    password(password), name(name)
{
}

QDataStream& operator<<(QDataStream& ds, const Hello& hello)
{
    ds << hello.password;
    ds << hello.name;
    return ds;
}

QDataStream& operator>>(QDataStream& ds, Hello& hello)
{
    ds >> hello.password;
    ds >> hello.name;
    return ds;
}

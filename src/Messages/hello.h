#ifndef HELLOCLIENT_H
#define HELLOCLIENT_H

#include <QDataStream>
#include <QByteArray>

struct Hello
{
    Hello() = default;
    explicit Hello(const QByteArray& password, const QString& name) noexcept;
    QByteArray password;
    QString name;
};
Q_DECLARE_METATYPE(Hello)

QDataStream& operator<<(QDataStream& ds, const Hello& hello);
QDataStream& operator>>(QDataStream& ds, Hello& hello);

#endif  // HELLOCLIENT_H

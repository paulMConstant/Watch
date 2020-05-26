#ifndef PASSWORDCONVENTIONS_H
#define PASSWORDCONVENTIONS_H

#include <QCryptographicHash>
#include <QByteArray>
#include <QString>

namespace PasswordConventions
{
[[nodiscard]] inline QByteArray hash(const QString& password) noexcept
{ return QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha512); }
}

#endif  // PASSWORD_H

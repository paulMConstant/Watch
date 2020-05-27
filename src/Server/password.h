#ifndef PASSWORD_H
#define PASSWORD_H

#include <QByteArray>

class Password
{
  public:
    Password() = default;
    Password(const QByteArray& password) noexcept;
    [[nodiscard]] bool operator==(const Password& other) const noexcept;
    void readFromUser() noexcept;

  private:
    QByteArray password;

    [[nodiscard]] QByteArray readConsole() noexcept;
    void setStdinEcho(bool enable) noexcept;
};

#endif  // PASSWORD_H
